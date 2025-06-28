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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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



uint8_t btree_midInsert(uint8_t pageBuffer[bt1_pagesize], uint8_t insKey[64], uint8_t insVal[64], uint8_t insPointer[4], int index){
    uint8_t switchKey[64], switchVal[64], switchPointer[4], nextKey[64], nextVal[64], nextPointer[4];
    for(int i = 0; i < bt1_keysize; i++){
        switchKey[i] = insKey[i];
        switchVal[i] = insVal[i];
        // if(bt1_debug){
        //     printf("%u", insKey[i]);
        // }
    }
    // if(bt1_debug){
    //     printf("\n");
    // }
    for(int i = 0; i < 4; i++){
        switchPointer[i] = insPointer[i];
    }
    for(int j = index; j < bt1_cellsperpage; j++){
        for(int i = 0; i < bt1_keysize; i++){ // Switch out the key
            nextKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
            pageBuffer[bt1_headersize+(bt1_cellsize*j)+i] = switchKey[i];
        }
        for(int i = 0; i < bt1_valsize; i++){ // Switch out the val
            nextVal[i] = pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*j)+i];
            pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*j)+i] = switchVal[i];
        }
        for(int i = 0; i < 4; i++){ // Switch out the pointer
            nextPointer[i] = pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*j)+i];
            pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*j)+i] = switchPointer[i];
        }
        // Load the switches with the next vals.
        for(int i = 0; i < bt1_keysize; i++){
            switchKey[i] = nextKey[i];
            switchVal[i] = nextVal[i];
        }
        for(int i = 0; i < 4; i++){
            switchPointer[i] = nextPointer[i];
        }
        if(bt1_debug){
            printf("Key %u:", j);
            for(int i = 0; i < 64; i++){
                printf("%u ", switchKey[i]);
            }
            printf("\n");
        }
    }
    // make the last pointer the end.
    for(int i = 0; i < 4; i++){
        pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)+i] = switchPointer[i];
    }
    // if(bt1_debug){
    //     for(int i = 0; i < bt1_pagesize; i++){
    //         printf("%u ", pageBuffer[i]);
    //     }
    //     printf("\n");
    // }
}
/**
 * @brief Finds the earliest free index to write a page to
 * @return The integer index of the writable page
 */
uint32_t btree_findfreepage(FILE* treeFile){
    uint8_t pageBuffer[bt1_pagesize];
    bool pageFree = false;
    fseek(treeFile, bt1_pagesize, SEEK_SET);
    int idx = 1; // page indices start at 1, we won' be checking the root.
    while(!pageFree && !feof(treeFile)){
        idx++;
        fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
        // check if free
        pageFree = true;
        for(int i = 0; i < bt1_pagesize; i++){
            pageFree = pageFree && pageBuffer[i] == 0;
        }
    }
    if(!pageFree && feof(treeFile)){
        // Reached EOF, write a free page before going EOF
        memset(pageBuffer, 0, 4096);
        fwrite(pageBuffer, sizeof(pageBuffer[0]), 4096, treeFile);
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

/**
 * @brief Convert an array of 4 bytes to a 64 bit unsigned integer big endian.
 * @return the 64 bit pointer integer.
 */
uint32_t btree_pointertoint(uint8_t pointer[4]){
    return (pointer[0] * (16*16*16)) + (pointer[1] * (16*16)) + (pointer[2] * 16) + pointer[3];
}

void btree_inttopointer(uint32_t pageIndex, uint8_t dest[4]){
    // 0 is the 16^3, 1 is 16^2, 2 is 16^1, 3 is 16^0
    // 16 32 48 64
    // 8  16 24 32
    dest[0] = pageIndex >> 24;
    dest[1] = (pageIndex << 8) >> 24;
    dest[2] = (pageIndex << 16) >> 24;
    dest[3] = (pageIndex << 24) >> 24;
}

/**
 * @brief Check if a given page needs a split if a node is added
 * @return 1 if a split is necessary, otherwise 0 if is unnecessary.
 */
int btree_checkPageNeedsSplit(uint32_t idx, FILE* treeFile){
    uint8_t pageBufferTemp[4096];
    uint8_t emptyKey[64];
    memset(emptyKey, 0, 64);
    fseek(treeFile, bt1_pagesize*(idx-1), SEEK_SET);
    fread(pageBufferTemp, sizeof(pageBufferTemp[0]), bt1_pagesize, treeFile);
    // header -> cell -> cell -> cell -> ... -> end pointer
    // We don't implement deletes so for now, we can just check if the LAST cell is filled.
    return 0 != btree_keycmp(&pageBufferTemp[bt1_headersize+(bt1_cellsize*(bt1_cellsperpage-1))], emptyKey);
}

/**
 * @brief For all children of a node, set their parent pointer to the given idx
 */
void btree_rectifyChildrenParents(uint32_t parentIdx, FILE* treeFile){
    uint8_t parentBuffer[bt1_pagesize], childBuffer[bt1_pagesize], nullKey[64], parentPointer[4];
    btree_inttopointer(parentIdx, parentPointer);
    memset(nullKey, 0, 64);
    uint32_t childIdx;
    fseek(treeFile, bt1_pagesize*(parentIdx-1), SEEK_SET);
    fread(parentBuffer, sizeof(parentBuffer[0]), bt1_pagesize, treeFile);
    if(bt1_debug){
        printf("Read the childbuffer, time to check the cells...\n");
    }
    for(int i = 0; i < bt1_cellsperpage; i++){
        childIdx = btree_pointertoint(&parentBuffer[bt1_headersize+(bt1_cellsize*i)+bt1_cellpointeroffset]);
        if(childIdx != 0){
            fseek(treeFile, bt1_pagesize*(childIdx-1), SEEK_SET);
            fread(childBuffer, sizeof(childBuffer[0]), bt1_pagesize, treeFile);
            for(int j = 0; j < 4; j++){
                childBuffer[j] = parentPointer[j];
            }
            fseek(treeFile, bt1_pagesize*(childIdx-1), SEEK_SET);
            fwrite(childBuffer, sizeof(childBuffer[0]), bt1_pagesize, treeFile);
        }
    }
}

uint32_t btree_splitBufferAndInsert(uint8_t pageBuffer[bt1_pagesize], uint8_t leftBuffer[bt1_pagesize], uint8_t rightBuffer[bt1_pagesize], uint8_t insKey[64], uint8_t insVal[64], uint8_t insPointer[4], uint8_t promKey[64], uint8_t promVal[64], uint8_t promPointer[4], int insIdx, FILE* treeFile){
    if(insIdx > (bt1_cellsperpage/2)){
        if(bt1_debug){
            printf("INSONRIGHT\n");
        }
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
        for(int j = (bt1_cellsperpage/2)+1; j < insIdx; j++){ // fill in easy buffer parts.
            for(int i = 0; i < bt1_cellsize; i++){
                rightBuffer[bt1_headersize+(bt1_cellsize*(j-((bt1_cellsperpage/2)+1)))+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
            }
        }
        // Fill in rest of the right buffer moving over each piece.
        uint8_t nextKey[64], nextVal[64], nextPointer[4], switchKey[64], switchVal[64], switchPointer[4];
        for(int i = 0; i < bt1_keysize; i++){
            switchKey[i] = insKey[i];
            switchVal[i] = insVal[i];
        }
        for(int i = 0; i < 4; i++){
            switchPointer[i] = insPointer[i];
        }
        for(int j = insIdx; j < bt1_cellsperpage; j++){
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
        for(int i = 0; i < 4; i++){ // fill in possible pointer
            rightBuffer[bt1_headersize+(bt1_cellsize*(bt1_cellsperpage-((bt1_cellsperpage/2)+1)))+i+bt1_cellpointeroffset] = switchPointer[i];
        }
        // put the value at idx 15 a layer up.
        for(int i = 0; i < bt1_keysize; i++){
            promKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*15)+i];
            promVal[i] = pageBuffer[bt1_headersize+(bt1_cellsize*15)+bt1_keysize+i];
        }
    } else if (insIdx == (bt1_cellsperpage/2)){
        if(bt1_debug){
            printf("INSONCENTER\n");
        }
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
        for(int i = 0; i < 4; i++){ // fill in possible pointer
            rightBuffer[bt1_headersize+(bt1_cellsize*(bt1_cellsperpage-((bt1_cellsperpage/2)+1)))+i+bt1_cellpointeroffset] = pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)+i];
        }
        // Set the prev and next pointer values for the centerpiece correctly.
        for(int i = 0; i < bt1_keysize; i++){
            promKey[i] = insKey[i];
            promVal[i] = insVal[i];
        }
    }
    else {
        // new value goes on the left
        if(bt1_debug){
            printf("INSONLEFT\n");
        }
        for(int i = 0; i < bt1_headersize; i++){
            leftBuffer[i] = pageBuffer[i];
            rightBuffer[i] = pageBuffer[i];
        }
        for(int j = 0; j < insIdx; j++){ // fill in easy buffer parts.
            for(int i = 0; i < bt1_cellsize; i++){
                leftBuffer[bt1_headersize+(bt1_cellsize*j)+i] = pageBuffer[bt1_headersize+(bt1_cellsize*j)+i];
            }
        }
        // Fill in rest of the left buffer moving over each piece.
        uint8_t nextKey[64], nextVal[64], nextPointer[4], switchKey[64], switchVal[64], switchPointer[4];
        for(int i = 0; i < bt1_keysize; i++){
            switchKey[i] = insKey[i];
            switchVal[i] = insVal[i];
        }
        for(int i = 0; i < 4; i++){
            switchPointer[i] = insPointer[i];
        }
        for(int j = insIdx; j < bt1_cellsperpage/2; j++){
            for(int i = 0; i < bt1_keysize; i++){ // Switch out the key
                nextKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*i)+i];
                leftBuffer[bt1_headersize+(bt1_cellsize*j)+i] = switchKey[i];
            }
            for(int i = 0; i < bt1_valsize; i++){ // Switch out the val
                nextVal[i] = pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+i];
                leftBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*j)+i] = switchVal[i];
            }
            for(int i = 0; i < 4; i++){ // Switch out the pointer
                nextPointer[i] = pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+i];
                leftBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*j)+i] = switchPointer[i];
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
        for(int i = 0; i < 4; i++){ // fill in possible pointer
            rightBuffer[bt1_headersize+(bt1_cellsize*(bt1_cellsperpage-((bt1_cellsperpage/2)+1)))+i+bt1_cellpointeroffset] = pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)+i];
        }
        for(int i = 0; i < bt1_keysize; i++){
            promKey[i] = pageBuffer[bt1_headersize+(bt1_cellsize*15)+i];
            promVal[i] = pageBuffer[bt1_headersize+(bt1_cellsize*15)+bt1_keysize+i];
        }
    }
    // find the next page index and pass it back as the return value.
    uint32_t ret = btree_findfreepage(treeFile);
    btree_inttopointer(ret, promPointer);
    return ret;
}

/**
 * @brief Finds the value of a given key if it exists in the btree, gives a pointer back to its temporary location in memory.
 * @return 1 if anything is found, otherwise 0. Return actual value to ret.
 */
int btree_findkey(FILE* treeFile, uint8_t key[64], uint8_t ret[64]){

    uint8_t pageBuffer[bt1_pagesize];
    // Load ROOT
    rewind(treeFile);
    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
    // Search ROOT for key OR next child pointer
    uint8_t comp_key[64], emptyKey[64]; // Comparison key, may not be necessary
    memset(emptyKey, 0, 64);
    while(true){ // search until an end is found
        int idx;
        // set the CMP key to the first key
        //fseek(treeFile, bt1_headersize, SEEK_SET);
        int cmp_result = 1;
        int emptycmp_result = 1;
        for(idx = 0; idx < bt1_cellsperpage; idx++){
            cmp_result = btree_keycmp(key, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            emptycmp_result = btree_keycmp(emptyKey, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            if(cmp_result <= 0 || emptycmp_result == 0){
                break;
            }
        }
        if(emptycmp_result == 0){
            // check if there is a valid pointer.
            uint32_t pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
            if(pageIdx == 0){
                return 0;
            } else {
                fseek(treeFile, pageIdx*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
            // the current slot is empty.
        } else if(idx < bt1_cellsperpage){
            // Stopped mid move, check if less than or equal
            if(cmp_result==0){
                // It's equal
                for(int i = 0; i < 64; i++){
                    ret[i] = pageBuffer[bt1_headersize+bt1_cellvalueoffeset+(bt1_cellsize*idx)+i];
                }
                return 1;
            } else {
                // It's less than the key, read the next page of memory
                uint32_t pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
                if(pageIdx == 0){
                    return 0; // Pointer is NULL, no key exists
                }
                fseek(treeFile, (pageIdx-1)*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
        } else {
            // Key is greater than all on this page, take the last pointer.
            uint32_t pageIdx = btree_pointertoint(&pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)]);
            if(pageIdx == 0){
                return 0; // Pointer is NULL, no key exists
            }
            fseek(treeFile, (pageIdx-1)*bt1_pagesize, SEEK_SET);
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
int btree_addvalue(FILE* treeFile, uint8_t key[64], uint8_t val[64], uint8_t prev[64]){

    if(bt1_debug){
        printf("Got to the start of ADDING\n");
    }
    // Load ROOT
    uint8_t pageBuffer[bt1_pagesize];
    bool stopLooking = false;
    bool keyExists;
    bool splitNeeded = false;
    // Load ROOT
    if(bt1_debug){
        printf("Time to seek! FP = %u\n", treeFile);
    }
    fseek(treeFile, 0, SEEK_SET);
    if(bt1_debug){
        printf("Time to read!\n");
    }
    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
    // Search ROOT for key OR next child pointer
    uint8_t comp_key[64], emptyKey[64]; // Comparison key, may not be necessary
    memset(emptyKey, 0, 64);
    int idx;
    uint32_t pageIdx = 1;
    if(bt1_debug){
        printf("Got to the start of looking...\n");
    }
    while(!stopLooking){ // search until an end is found
        // set the CMP key to the first key
        //fseek(treeFile, bt1_headersize, SEEK_SET);
        if(bt1_debug){
            printf("Looking through another page.\n");
        }
        int cmp_result = 1;
        int emptycmp_result = 1;
        for(idx = 0; idx < bt1_cellsperpage; idx++){
            cmp_result = btree_keycmp(key, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            emptycmp_result = btree_keycmp(emptyKey, &pageBuffer[bt1_headersize+(bt1_cellsize*idx)]);
            if(bt1_debug){
                printf("CMPresult = %u, emptyCMP_result = %u\n", cmp_result, emptycmp_result);
            }
            if(cmp_result <= 0 || emptycmp_result == 0){
                break;
            }
        }
        if(emptycmp_result == 0){
            // check if there is a valid pointer.
            uint32_t pageIdxtmp = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
            //printf("idx %i yielded pageidx %u\n", idx, pageIdxtmp);
            if(pageIdxtmp == 0){
                stopLooking = true;
                keyExists = false; // Pointer is NULL, no key exists
            } else {
                fseek(treeFile, (pageIdxtmp-1)*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
            // the current slot is empty.
        } else if(idx < bt1_cellsperpage){
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
                uint32_t pageIdxtmp = btree_pointertoint(&pageBuffer[bt1_headersize+bt1_cellpointeroffset+(bt1_cellsize*idx)]);
                if(pageIdxtmp == 0){
                    stopLooking = true;
                    keyExists = false; // Pointer is NULL, no key exists
                } else {
                    fseek(treeFile, pageIdxtmp*bt1_pagesize, SEEK_SET);
                    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
                }
            }
        } else {
            // Key is greater than all on this page, take the last pointer.
            uint32_t pageIdxtmp = btree_pointertoint(&pageBuffer[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)]);
            if(pageIdxtmp == 0){
                stopLooking = true;
                keyExists = false; // Pointer is NULL, no key exists
                splitNeeded = true; // Keys are full, must split.
            } else {
                fseek(treeFile, pageIdxtmp*bt1_pagesize, SEEK_SET);
                fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            }
        }
    }
    if(bt1_debug){
        printf("Done looking, keyexists = %u, splitneeded = %u\n", keyExists, splitNeeded);
    }
    if(keyExists){
        // no need to check if split necessary, just write the buffer
        fseek(treeFile, (pageIdx-1)*bt1_pagesize, SEEK_SET);// Reset file position to beginning of page
        fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile); // Write to page
    } else {
        // If split necessity unknown, check entire page for space
        if(!splitNeeded){ // check if split necessary.
            splitNeeded = btree_checkPageNeedsSplit(pageIdx, treeFile);
            if(bt1_debug){
                printf("Checked again, splitneeded = %u\n", splitNeeded);
            }
        }
        if(!splitNeeded){
            // split still not needed, add key and shift all values right.
            uint8_t emptyPointer[4];
            memset(emptyPointer, 0, 4);
            btree_midInsert(pageBuffer, key, val, emptyPointer, idx); // formal error
            fseek(treeFile, (pageIdx-1)*bt1_pagesize, SEEK_SET);// Reset file position to beginning of page
            fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile); // Write to page
            // if(bt1_debug){
            //     for(int i = 0; i < bt1_keysize; i++){
            //         printf("%u ", key[i]);
            //     }
            //     printf("\n");
            //     for(int i = 0; i < bt1_pagesize; i++){
            //         printf("%u ", pageBuffer[i]);
            //     }
            //     printf("\n");
            //     printf("Inserting it at pageindex %u, cellindex %u, writing...\n", pageIdx, idx);
            // }
        } else {
            // split needed, determine whether new key stays on page or moves
            // then perform the split.
            
            // idx 0-14 goes on the left
            // idx 15 goes in the middle
            // idx 16-30 goes on the right
            
            // there may need to be a secondary version of this method for the case of splitting the root.
            uint8_t splitKey[64], splitVal[64], splitPointer[4], promKey[64], promVal[64], newPointer[4], promPrev[4];
            memset(splitPointer, 0, 4);
            for(int i = 0; i < 64; i++){
                splitKey[i] = key[i];
                splitVal[i] = val[i];
            }
            uint32_t parentIdx, freeIdx;
            int splitIdx = idx;

            uint8_t leftBuffer[bt1_pagesize], rightBuffer[bt1_pagesize];
            memset(leftBuffer, 0, bt1_pagesize);
            memset(rightBuffer, 0, bt1_pagesize);
            while(splitNeeded){
                freeIdx = btree_splitBufferAndInsert(pageBuffer, leftBuffer, rightBuffer, splitKey, splitVal, splitPointer, promKey, promVal, newPointer, splitIdx, treeFile);
                if(pageIdx != 1){ // Not a ROOT, split normally.
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
                    // Figure out who the lucky parent of this split is.
                    parentIdx = btree_pointertoint(pageBuffer);
                    splitNeeded = btree_checkPageNeedsSplit(parentIdx, treeFile);
                    // Write the pages, determine pointers, and insert into the above page.
                    fseek(treeFile, bt1_pagesize*(pageIdx-1), SEEK_SET);
                    fwrite(rightBuffer, sizeof(rightBuffer[0]), bt1_pagesize, treeFile);
                    fseek(treeFile,bt1_pagesize*(freeIdx-1), SEEK_SET);
                    fwrite(leftBuffer, sizeof(leftBuffer[0]), bt1_pagesize, treeFile);
                    btree_rectifyChildrenParents(freeIdx, treeFile); // Left buffer needs to have its children rectified.
                    fseek(treeFile, bt1_pagesize*(parentIdx-1), SEEK_SET);
                    fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
                    // Find which cell has the before pointer equal to the current page's index
                    // TODO: Is it possible the split value may end up on the outside?
                    int cellIdx = 0;
                    while(btree_pointertoint(&pageBuffer[bt1_headersize+(cellIdx*bt1_cellsize)+bt1_cellpointeroffset]) != pageIdx){
                        // check cells.
                        cellIdx++;
                        if(cellIdx > bt1_cellsperpage){
                            printf("Page at index %u has no pointer in parent!", pageIdx);
                            return -1;
                        }
                    }
                    if(!splitNeeded){
                        // repeat code from no-split insertion
                        btree_midInsert(pageBuffer, promKey, promVal, newPointer, cellIdx); // formal error
                        // Write new parent.
                        fseek(treeFile, bt1_pagesize*(parentIdx-1), SEEK_SET);// Reset file position to beginning of page
                        fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile); // Write to page
                        }
                    else {
                        // set the promoted values as the split values
                        for(int i = 0; i < 64; i++){
                            splitKey[i] = promKey[i];
                            splitVal[i] = promVal[i];
                        }
                        for(int i = 0; i < 4; i++){
                            splitPointer[i] = newPointer[i];
                        }
                        pageIdx = parentIdx;
                        // Loop back to the beginning, perform the split procedure.
                        // Check if current page is ROOT, special case.
                        
                    }
                } else { // We are at the ROOT! Special split procedure.
                    if(bt1_debug){
                        printf("Commencing root split procedure...\n");
                    }
                    // Make sure that the new buffers have parent pointers to 0 0 0 1
                    uint8_t rootPointer[4] = {0,0,0,1};
                    for(int i = 0; i < 4; i++){
                        leftBuffer[i] = rootPointer[4];
                        rightBuffer[i] = rootPointer[4];
                    }
                    // Write the new page as normal, then find a NEW free page for the old page.
                    // Write the ROOT as a new single entry node of the promoted key.
                    // Make sure to go back and implement the possibility of full cell followed by empty cell with pointer
                    if (bt1_debug){
                        printf("Writing buffers... FP = %i\n", treeFile);
                    }
                    fseek(treeFile,bt1_pagesize*(freeIdx-1), SEEK_SET);
                    fwrite(leftBuffer, sizeof(leftBuffer[0]), bt1_pagesize, treeFile);
                    fflush(treeFile);
                    uint32_t secondFreeIdx = btree_findfreepage(treeFile);
                    uint8_t secondFreePointer[4];
                    btree_inttopointer(secondFreeIdx, secondFreePointer);
                    fseek(treeFile,bt1_pagesize*(secondFreeIdx-1), SEEK_SET);
                    fwrite(leftBuffer, sizeof(leftBuffer[0]), bt1_pagesize, treeFile);
                    // rectify both new nodes
                    if(bt1_debug){
                        printf("Rectifying Children of new pages %u and %u...\n", freeIdx, secondFreeIdx);
                    }
                    btree_rectifyChildrenParents(freeIdx, treeFile);
                    btree_rectifyChildrenParents(secondFreeIdx, treeFile);
                    //newPointer is the LEFT, secondFreePointer is the RIGHT
                    if(bt1_debug){
                        printf("Creating new root... FP = %u\n", treeFile);
                    }
                    memset(pageBuffer, 0, bt1_pagesize);
                    for(int i = 0; i < bt1_keysize; i++){
                        pageBuffer[bt1_headersize+i] = promKey[i];
                        pageBuffer[bt1_headersize+i+bt1_cellvalueoffeset] = promVal[i];
                    }
                    for(int i = 0; i< 4; i++){
                        pageBuffer[bt1_headersize+bt1_cellpointeroffset+i] = newPointer[i];
                        pageBuffer[bt1_headersize+bt1_cellpointeroffset+i+bt1_cellsize] = secondFreePointer[i];
                    }
                    if(bt1_debug){
                        printf("Writing new root... FP = %u\n", treeFile);
                    }
                    fseek(treeFile, 0, SEEK_SET);
                    fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
                    splitNeeded = false;
                    if(bt1_debug){
                        printf("Postwrite FP = %u\n", treeFile);
                        printf("leftBuffer: ");
                        for(int i = 0; i < 4; i++){
                            printf("%u ", leftBuffer[i]);
                        }
                        printf("\n");
                        for(int i = 0; i < bt1_cellsperpage; i++){
                            for(int j = 0; j < bt1_keysize; j++){
                                printf("%u ", leftBuffer[bt1_headersize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < bt1_valsize; j++){
                                printf("%u ", leftBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < 4; j++){
                                printf("%u ", leftBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                        }
                        for(int i = 0; i < 4; i++){
                            printf("%u ", leftBuffer[bt1_headersize+(bt1_cellsperpage*bt1_cellsize)+i]);
                        }
                        printf("\n");
                        printf("rightBuffer: ");
                        for(int i = 0; i < 4; i++){
                            printf("%u ", rightBuffer[i]);
                        }
                        printf("\n");
                        for(int i = 0; i < bt1_cellsperpage; i++){
                            for(int j = 0; j < bt1_keysize; j++){
                                printf("%u ", rightBuffer[bt1_headersize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < bt1_valsize; j++){
                                printf("%u ", rightBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < 4; j++){
                                printf("%u ", rightBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                        }
                        for(int i = 0; i < 4; i++){
                            printf("%u ", rightBuffer[bt1_headersize+(bt1_cellsperpage*bt1_cellsize)+i]);
                        }
                        printf("\n");
                        printf("pageBuffer: ");
                        for(int i = 0; i < 4; i++){
                            printf("%u ", pageBuffer[i]);
                        }
                        printf("\n");
                        for(int i = 0; i < bt1_cellsperpage; i++){
                            for(int j = 0; j < bt1_keysize; j++){
                                printf("%u ", pageBuffer[bt1_headersize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < bt1_valsize; j++){
                                printf("%u ", pageBuffer[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                            for(int j = 0; j < 4; j++){
                                printf("%u ", pageBuffer[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+j]);
                            }
                            printf("\n");
                        }
                        for(int i = 0; i < 4; i++){
                            printf("%u ", pageBuffer[bt1_headersize+(bt1_cellsperpage*bt1_cellsize)+i]);
                        }
                        printf("\n");
                    }
                    /*
                     * Current problems shown by this:
                     * pageBuffer has two pointers to page 4, despite the children supposedly being page 4 and 5
                     * Pointers are randomly interspersed in the left and right buffers when they shouldn't be
                     * 
                    */
                }
            }
        }
    }
    //printf("Preflush fp = %u\n", treeFile);
    int flushresult = fflush(treeFile);
    if(bt1_debug){
        printf("Flush result: %i, FP = %u\n",flushresult, treeFile);
    }
    return 1;
    // Search ROOT for key OR next child pointer

    // Traverse down until LEAF is reached without success or until the KEY is found

    // If no key is found, insert key on LEAF and split if necessary (see split pngs), set prev to NULL and return 0

      // SPLIT PROCEDURE
       // LEAF NODES: Split the leaf in half, first key of the second half is added to the parent with an after pointer to the others
        // Second half keys and parent idx need to stay in-memory after deletion of second half from OG and switching to new unoccupied page. Splitting key and indexes of both pages must stay in-memory when switching to parent.
       // NONLEAF NODES: Split the leaf in half, first key of the secon d half is addeed to the parent, original after pointer becomes the before pointer of new node.

    // Else, if a key is found replace the value at the key and set prev to the value, return 1
}

int main(void){
    printf("We running with it!\n");
    // Lets create a basic file to work with.
    FILE* fp = fopen("test.tmp","w+");
    printf("Result of attempting to create a new db: %u\n", btree_createNewDB(fp));
    uint8_t addKey[64], addVal[64], addPrev[64], addRet[64];
    memset(addKey, 1, 64);
    memset(addVal, 2, 64);
    memset(addPrev, 0, 64);
    memset(addRet, 0, 64);
    printf("Result of attempting to add a new key and val: %u\n", btree_addvalue(fp, addKey, addVal, addPrev));
    printf("Result of attempting to find the new key: %u\n", btree_findkey(fp, addKey, addRet));
    bool isCorrect = true;
    for(int i = 0; i < 64; i++){
        isCorrect = isCorrect && addVal[i] == addRet[i];
    }
    if(isCorrect){
        printf("The value was returned correctly!\n");
    }
    printf("Result of attempting to readd a new key and val: %u\n", btree_addvalue(fp, addKey, addVal, addPrev));
    isCorrect = true;
    for(int i = 0; i < 64; i++){
        isCorrect = isCorrect && addVal[i] == addPrev[i];
    }
    if(isCorrect){
        printf("The prev value was returned correctly!\n");
    }
    //fclose(fp);
    time_t startTime = time(NULL);
    srand(time(NULL));
    for(long long i = 0; i < 20000; i++){
        //FILE* fp = fopen("test.tmp","r+");
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        printf("%lli/20000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(fp, addKey, addVal, addPrev));
    }
    printf("\nInserting 20000 random values took %.f seconds.", startTime - time(NULL));
    fclose(fp);
}

