/* Btree project
 *
 * 4kb pages (4096 bytes)
 * Keys and Values are 64 bytes
 *
 *---------------------------------------------
 * 
 * Calculations for page size (from lecture).
 *
 * Header: 4 byte pointer
 *
 * Each cell is:
 *  - 64 byte key
 *  - 64 byte value
 *  - 4 byte child pointer
 *  - 132 bytes per
 *  *30 = 3960 bytes
 *
 * 4 byte tail pointer
 *
 * = 3964 bytes per page
 *
 * Rest is filled with NULL bytes for padding.
 *
 * --------------------------------------------
 * 
 * For full credit, pages must be stored in a file and one page loaded into memory at a time ( a buffer of 4096 bytes).
 * 
 * NULL pointer is -1
 *
 * If less than key and greater than previous key, take child pointer of the current key.
 * If greater than all in node, take the tail pointer
 * If the key is in the node, use the cell's value.
 *
 * Splits
 *
 * If 2d is exceeded, create two nodes and use the middle key as the differentiator in the parent node
 * 
 * Look in the book for a better algorithmic explanation
 * 
 * Merges
 * If below d, redistribute keys among the other nodes.
 * 
 * Look in the book for a better algorithmic explanation.
 * 
 * --------------------------------------------
 *
 * Asking for constraints in class
 *
 * Memory map?
 * -> What retert did in his own implementation of the project. 
 * -> man mmap (it's in the C std library)
 * -> How to represent the file in the program. 
 *
 * Amount of pages in memory
 * -> Theoretically allowed to have all the pages you have traversed inside memory at once.
 * -> May be a good idea to just have one page at a time for simplicity sake, but will make traversal slower (more overhead).
 *
 * --------------------------------------------
 * 
 * Traversal and you!
 *
 * Each 'pointer' is an index in the page file. In order to get to that index, you simply need to go the fd at byte pagesize*index. Then, read in the entire pagesize.
 *
 * Intracell traversal can be done rather easily in these steps
 * -> The first cell will be at index headersize.
 * -> Each progressive cell starts at headersize+(cellsize*cellidx).
 * -> Use the respective offsets to increment from this starting point to read the various values
 *    * Key is [keyoffset:valueoffset]
 *    * Value is [valueoffset:pointeroffset]
 *    * Pointer Offset is [pointeroffset:cellsize]
 * -> Remember! The pointer in a cell is for values BEFORE the key value, not after. If all are before, the final pointer at the end will be the after pointer.
 * 
 * If you intend on keeping track of the links of pages you went through to get to a page (instead of just using the header parent pointer), you may want to create
 *   a simple linked list structure that can automate the malloc and freeing for you!
 *
 *
 * */


#include "btreeproj.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Useful Documentation
 * Opening files general guide: https://www.w3schools.com/c/c_files_read.php
 * IO command reference: https://en.cppreference.com/w/c/io.html
 * 
 * 
 * fseek with SEEK_SET will be useful for changing between nodes
 * Remember to open files in BINARY mode
 * fsync can be used to force writes back to disk. 
 */

/**
 * @brief Finds the earliest free index to write a page to
 * @return The integer index of the writable page
 */
long btree_findfreepage(FILE* treeFile){
    uint8_t pageBuffer[bt1_pagesize];
    bool pageFree = false;
    int idx = 0; // page indices start at 1.
    while(!pageFree){
        idx++;
        fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
        // check if free
        if(true){
            pageFree = true;
        }
    }
    return idx;
}

/**
 * @brief Initializes the root for a new database in the given file.
 * @return 1 if successful, otherwise 0.
 */
int btree_createNewDB(FILE* treeFile){
    // db has special parent header NULL (0)
    uint8_t pageBuffer[bt1_pagesize];
    for(int i = 0; i < bt1_pagesize; i++){
        pageBuffer[i] = 0; 
    }
    fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
    return 1;
}

/**
 * @brief Compare arrays
 * @return 1 if key1 is greater than key2, 0 if keys are equal, -1 if key1 is less than key2
 */
int btree_keycmp(uint8_t key1[64], uint8_t key2[64]){
    for(int i = 0; i < 64; i++){
        if(key1[i] > key2[i]){
            return 1;
        } else if (key1[i] < key2[i]){
            return -1;
        }
    }
    return 0;
}

uint64_t btree_pointertoint(uint8_t pointer[4]){
    return (pointer[0] * (16*16*16)) + (pointer[1] * (16*16)) + (pointer[2] * 16) + pointer[3];
}


/**
 * @brief Finds the value of a given key if it exists in the btree, gives a pointer back to its temporary location in memory.
 * @return 1 if anything is found, otherwise 0. Return actual value to ret.
 */
int btree_findkey(int* buffer, FILE* treeFile, uint8_t key[64], uint8_t ret[64]){

    uint8_t pageBuffer[bt1_pagesize];
    // Load ROOT
    rewind(treeFile);
    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
    // Search ROOT for key OR next child pointer
    uint8_t comp_key[64]; // Comparison key, may not be necessary
    while(true){ // search until an end is found
        int idx;
        // set the CMP key to the first key
        //fseek(treeFile, bt1_headersize, SEEK_SET);
        int cmp_result = 1;
        for(idx = 0; idx < bt1_cellsperpage; idx++){
            cmp_result = btree_keycmp(key, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            if(cmp_result <= 0){
                break;
            }
        }
        if(idx < bt1_cellsperpage){
            // Stopped mid move, check if less than or equal
            if(cmp_result==0){
                // It's equal
                for(int i = 0; i < 64; i++){
                    ret[i] = pageBuffer[bt1_headersize+bt1_cellvalueoffeset+(bt1_cellsize*idx)+i];
                }
                return 1;
            } else {
                // It's less than the key, read the next page of memory
                long pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
                if(pageIdx == 0){
                    return 0; // Pointer is NULL, no key exists
                }
                fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
        } else {
            // Key is greater than all on this page, take the last pointer.
            long pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)]);
            if(pageIdx == 0){
                return 0; // Pointer is NULL, no key exists
            }
            fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);
            fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
        }
}
    // Traverse down until LEAF is reached without success or until the KEY is found

    // If no key is found, return 0 and set ret to NULL

    // Else, if a key is found, copy the value to ret and return 1.
}

/**
 * @brief Add a value to the DB in the specified filedesc, prev will return the previous value if it exists, otherwise will become NULL
 * @return 1 if key replace, 0 if key added, -1 if key can't be added
 */
int btree_addvalue(int* buffer, FILE* treeFile, uint8_t key[64], uint8_t val[64], uint8_t prev[64]){
    // Load ROOT
    uint8_t pageBuffer[bt1_pagesize];
    bool stopLooking = false;
    bool keyExists;
    bool splitNeeded = false;
    // Load ROOT
    rewind(treeFile);
    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
    // Search ROOT for key OR next child pointer
    uint8_t comp_key[64]; // Comparison key, may not be necessary
    int idx;
    long pageIdx = 0;
    while(!stopLooking){ // search until an end is found
        // set the CMP key to the first key
        //fseek(treeFile, bt1_headersize, SEEK_SET);
        int cmp_result = 1;
        for(idx = 0; idx < bt1_cellsperpage; idx++){
            cmp_result = btree_keycmp(key, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            if(cmp_result <= 0){
                break;
            }
        }
        if(idx < bt1_cellsperpage){
            // Stopped mid move, check if less than or equal
            if(cmp_result==0){
                // It's equal
                for(int i = 0; i < 64; i++){
                    prev[i] = pageBuffer[bt1_headersize+bt1_cellvalueoffeset+(bt1_cellsize*idx)+i];
                    pageBuffer[bt1_headersize+bt1_cellvalueoffeset+(bt1_cellsize*idx)+i] = val[i];
                }
                stopLooking = true;
                keyExists = true;
            } else {
                // It's less than the key, read the next page of memory
                pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
                if(pageIdx == 0){
                    stopLooking = true;
                    keyExists = false; // Pointer is NULL, no key exists
                } else {
                    fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);
                    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
                }
            }
        } else {
            // Key is greater than all on this page, take the last pointer.
            pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)]);
            if(pageIdx == 0){
                stopLooking = true;
                keyExists = false; // Pointer is NULL, no key exists
                splitNeeded = true; // Keys are full, must split.
            } else {
                fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
        }
    }
    if(keyExists){
        // no need to check if split necessary, just write the buffer
        fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);// Reset file position to beginning of page
        fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile); // Write to page
    } else {
        // If split necessity unknown, check entire page for space
        if(!splitNeeded){ // check if split necessary.
            int count = 0;
            uint8_t nullKey[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            for(int i = 0; i < bt1_cellsperpage; i++){
                if(btree_keycmp(&pageBuffer[bt1_headersize+(bt1_cellsize*i)], nullKey)!=0){
                    count++;
                }
            }
            splitNeeded = count >= bt1_cellsperpage;
        }
        if(!splitNeeded){
            // split still not needed, add key and shift all values right.
            uint8_t nextKey[64], nextVal[64], nextPointer[4], switchKey[64], switchVal[64], switchPointer[4];
            for(int i = 0; i < bt1_keysize; i++){
                switchKey[i] = key[i];
                switchVal[i] = val[i];
            }
            for(int i = 0; i < 4; i++){
                switchPointer[i] = 0;
            }
            for(int j = idx; j < bt1_cellsperpage; j++){
                for(int i = 0; i < bt1_keysize; i++){ // Switch out the key
                    nextKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*i)+i];
                    pageBuffer[bt1_headersize+(bt1_cellsize*i)+i] = switchKey[i];
                }
                for(int i = 0; i < bt1_valsize; i++){ // Switch out the val
                    nextVal[i] = pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i];
                    pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i] = switchVal[i];
                }
                for(int i = 0; i < 4; i++){ // Switch out the pointer
                    nextPointer[i] = pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i];
                    pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i] = switchPointer[i];
                }
                // Load the switches with the next vals.
                for(int i = 0; i < bt1_keysize; i++){
                    switchKey[i] = nextKey[i];
                    switchVal[i] = nextVal[i];
                }
                for(int i = 0; i < 4; i++){
                    switchPointer[i] = nextPointer[i];
                }
            }
            fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);// Reset file position to beginning of page
            fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile); // Write to page
        } else {
            // split needed, determine whether new key stays on page or moves
            // then perform the split.
            
            // idx 0-14 goes on the left
            // idx 15 goes in the middle
            // idx 16-30 goes on the right
            
            // there may need to be a secondary version of this method for the case of splitting the root.

            uint8_t leftBuffer[bt1_pagesize], rightBuffer[bt1_pagesize];
            if(idx > (bt1_cellsperpage/2)){
                // new value goes on the right
                for(int i = 0; i < bt1_headersize; i++){
                    leftBuffer[i] = pageBuffer[i];
                    rightBuffer[i] = pageBuffer[i];
                }
                for(int j = 0; j <bt1_cellsperpage/2; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        leftBuffer[bt1_headersize+(bt1_cellsize*j)+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                for(int j = (bt1_cellsperpage/2)+1; j < idx; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        rightBuffer[bt1_headersize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                // Fill in rest of the right buffer moving over each piece.
                uint8_t nextKey[64], nextVal[64], nextPointer[4], switchKey[64], switchVal[64], switchPointer[4];
                for(int i = 0; i < bt1_keysize; i++){
                    switchKey[i] = key[i];
                    switchVal[i] = val[i];
                }
                for(int i = 0; i < 4; i++){
                    switchPointer[i] = 0;
                }
                for(int j = idx; j < bt1_cellsperpage; j++){
                    for(int i = 0; i < bt1_keysize; i++){ // Switch out the key
                        nextKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*i)+i];
                        rightBuffer[bt1_headersize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = switchKey[i];
                    }
                    for(int i = 0; i < bt1_valsize; i++){ // Switch out the val
                        nextVal[i] = pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i];
                        rightBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = switchVal[i];
                    }
                    for(int i = 0; i < 4; i++){ // Switch out the pointer
                        nextPointer[i] = pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i];
                        rightBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = switchPointer[i];
                    }
                    // Load the switches with the next vals.
                    for(int i = 0; i < bt1_keysize; i++){
                        switchKey[i] = nextKey[i];
                        switchVal[i] = nextVal[i];
                    }
                    for(int i = 0; i < 4; i++){
                        switchPointer[i] = nextPointer[i];
                    }
                }
                // put the value at idx 15 a layer up.

            } else if (idx == (bt1_cellsperpage/2)){
                // new value is the centerpiece
                for(int i = 0; i < bt1_headersize; i++){
                    leftBuffer[i] = pageBuffer[i];
                    rightBuffer[i] = pageBuffer[i];
                }
                for(int j = 0; j <bt1_cellsperpage/2; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        leftBuffer[bt1_headersize+(bt1_cellsize*j)+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                for(int j = (bt1_cellsperpage/2)+1; j < bt1_cellsperpage; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        rightBuffer[bt1_headersize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                // Set the prev and next pointer values for the centerpiece correctly.
            } else {
                // new value goes on the left
                for(int i = 0; i < bt1_headersize; i++){
                    leftBuffer[i] = pageBuffer[i];
                    rightBuffer[i] = pageBuffer[i];
                }
                for(int j = 0; j < idx; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        leftBuffer[bt1_headersize+(bt1_cellsize*j)+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                // Fill in rest of the left buffer moving over each piece.
                uint8_t nextKey[64], nextVal[64], nextPointer[4], switchKey[64], switchVal[64], switchPointer[4];
                for(int i = 0; i < bt1_keysize; i++){
                    switchKey[i] = key[i];
                    switchVal[i] = val[i];
                }
                for(int i = 0; i < 4; i++){
                    switchPointer[i] = 0;
                }
                for(int j = idx; j < bt1_cellsperpage/2; j++){
                    for(int i = 0; i < bt1_keysize; i++){ // Switch out the key
                        nextKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*i)+i];
                        leftBuffer[bt1_headersize+(bt1_cellsize*i)+i] = switchKey[i];
                    }
                    for(int i = 0; i < bt1_valsize; i++){ // Switch out the val
                        nextVal[i] = pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i];
                        leftBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i] = switchVal[i];
                    }
                    for(int i = 0; i < 4; i++){ // Switch out the pointer
                        nextPointer[i] = pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i];
                        leftBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i] = switchPointer[i];
                    }
                    // Load the switches with the next vals.
                    for(int i = 0; i < bt1_keysize; i++){
                        switchKey[i] = nextKey[i];
                        switchVal[i] = nextVal[i];
                    }
                    for(int i = 0; i < 4; i++){
                        switchPointer[i] = nextPointer[i];
                    }
                }
                // Fill in the right buffer.
                for(int j = (bt1_cellsperpage/2)+1; j < bt1_cellsperpage; j++){ // fill in easy buffer parts.
                    for(int i = 0; i < bt1_cellsize; i++){
                        rightBuffer[bt1_headersize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
                    }
                }
                // Add the former (now in the NEXT spot) key at idx 15 to the one above, reference split2.png
            }
            /* Now that the left and right buffers have been settled, we must do some simple things.
             * 
             * Ensure the key, value, and needed indices are in choice variables
             * promKey
             * promVal
             * promPrev
             * promNext
             * 
             * We must choose whether to make the new page the PREV or NEXT index.
             * -> based on the book, the 'right' page retains the old index number and the 'left' is the new one.
             * The other index, key, and value will be specified by the methods previous that set the buffers
             * The new page must be found as empty (fully null with no parent pointer) and then saved to.
             * 
             * With some work, the above method can be made into a loop to continue this sequence until root is eventually found.
             * 
             * Otherwise, a secondary (and much heavier) version for non-leafs can be written below.
             */
        }
    }
        
    // Search ROOT for key OR next child pointer

    // Traverse down until LEAF is reached without success or until the KEY is found

    // If no key is found, insert key on LEAF and split if necessary (see split pngs), set prev to NULL and return 0

      // SPLIT PROCEDURE
       // LEAF NODES: Split the leaf in half, first key of the second half is added to the parent with an after pointer to the others
        // Second half keys and parent idx need to stay in-memory after deletion of second half from OG and switching to new unoccupied page. Splitting key and indexes of both pages must stay in-memory when switching to parent.
       // NONLEAF NODES: Split the leaf in half, first key of the secon d half is addeed to the parent, original after pointer becomes the before pointer of new node.

    // Else, if a key is found replace the value at the key and set prev to the value, return 1
}

