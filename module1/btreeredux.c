#include "btreeproj.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


btbuffer_printbuffer(uint8_t pb[bt1_pagesize]){
    printf("pageBuffer: ");
    for(int i = 0; i < 4; i++){
        printf("%u ", pb[i]);
    }
    printf("\n");
    for(int i = 0; i < bt1_cellsperpage; i++){
        for(int j = 0; j < bt1_keysize; j++){
            printf("%u ", pb[bt1_headersize+(bt1_cellsize*i)+j]);
        }
        printf("\n");
        for(int j = 0; j < bt1_valsize; j++){
            printf("%u ", pb[bt1_headersize+bt1_keysize+(bt1_cellsize*i)+j]);
        }
        printf("\n");
        for(int j = 0; j < 4; j++){
            printf("%u ", pb[bt1_headersize+bt1_keysize+bt1_valsize+(bt1_cellsize*i)+j]);
        }
        printf("\n");
    }
    for(int i = 0; i < 4; i++){
        printf("%u ", pb[bt1_headersize+(bt1_cellsperpage*bt1_cellsize)+i]);
    }
    printf("\n");
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
    if(bt1_debug){
        printf("NewPageIdx=%u",idx);
    }
    return idx;
}

/**
 * @brief Finds the earliest free index to write a page to
 * @return The integer index of the writable page
 */
uint32_t btree_findfreepageMEM(uint8_t* mb){
    uint8_t pageBuffer[bt1_pagesize];
    bool pageFree = false;
    int idx = 1; // page indices start at 1, we won' be checking the root.
    while(!pageFree && idx < bt1_memorypages){
        idx++;
        for(int i = 0; i < bt1_pagesize; i++){
            pageBuffer[i] = mb[(bt1_pagesize*((idx)-1))+i];
        }
        // check if free
        pageFree = true;
        for(int i = 0; i < bt1_pagesize; i++){
            pageFree = pageFree && pageBuffer[i] == 0;
        }
    }
    if(!pageFree && idx == bt1_memorypages){
        return 0;
        printf("OUTOFMEMORY, EXIT IMMEDIATELY\n")
    }
    if(bt1_debug){
        printf("NewPageIdx=%u",idx);
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
 * @brief Convert an array of 4 bytes to a 64 bit unsigned integer big endian.
 * @return the 64 bit pointer integer.
 */
uint32_t btree_pointertoint(uint8_t pointer[4]){
    // for(int i = 0; i < 4; i++){
    //     printf("%u ", pointer[i]);
    // }
    // printf("\n");
    return (pointer[0] * (256*256*256)) + (pointer[1] * (256*256)) + (pointer[2] * 256) + pointer[3];
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
 * @brief For all children of a node, set their parent pointer to the given idx
 */
void btree_rectifyChildrenParentsMEM(uint32_t parentIdx, uint8_t* mb){
    uint8_t parentBuffer[bt1_pagesize], childBuffer[bt1_pagesize], nullKey[64], parentPointer[4];
    btree_inttopointer(parentIdx, parentPointer);
    memset(nullKey, 0, 64);
    uint32_t childIdx;
    for(int i = 0; i < bt1_pagesize; i++){
        parentBuffer[i] = mb[(bt1_pagesize*((parentIdx)-1))+i];
    }
    if(bt1_debug){
        printf("Read the childbuffer, time to check the cells...\n");
    }
    for(int i = 0; i < bt1_cellsperpage+1; i++){
        if(i < bt1_cellsperpage){
            childIdx = btree_pointertoint(&parentBuffer[bt1_headersize+(bt1_cellsize*i)+bt1_cellpointeroffset]);
        }
        else{
            childIdx = btree_pointertoint(&parentBuffer[bt1_headersize+(bt1_cellsize*i)]);
        }
        if(childIdx != 0){
            for(int i = 0; i < bt1_pagesize; i++){
                childBuffer[i] = mb[(bt1_pagesize*((childIdx)-1))+i];
            }
            for(int j = 0; j < 4; j++){
                childBuffer[j] = parentPointer[j];
            }
            for(int i = 0; i < bt1_pagesize; i++){
                mb[(bt1_pagesize*((childIdx)-1))+i] = childBuffer[i];
            }
        }
    }
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
    for(int i = 0; i < bt1_cellsperpage+1; i++){
        if(i < bt1_cellsperpage){
            childIdx = btree_pointertoint(&parentBuffer[bt1_headersize+(bt1_cellsize*i)+bt1_cellpointeroffset]);
        }
        else{
            childIdx = btree_pointertoint(&parentBuffer[bt1_headersize+(bt1_cellsize*i)]);
        }
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

/**
 * @brief Write a cell into a pagebuffer.
 */
void btbuffer_writecell(uint8_t* pb, uint8_t cell[bt1_cellsize], int cellIdx){
    for(int i = bt1_headersize+(bt1_cellsize*cellIdx); i < bt1_headersize+(bt1_cellsize*cellIdx)+bt1_cellsize; i++){
        pb[i] = cell[i-(bt1_headersize+(bt1_cellsize*cellIdx))];
    }
}

/**
 * @brief Write an after pointer into a pagebuffer.
 */
void btbuffer_writeapointer(uint8_t* pb, uint8_t p[4], int cellIdx){
    if(cellIdx < bt1_cellsperpage){
        for(int i = 0; i < 4; i++){
            pb[bt1_headersize+(bt1_cellsize*cellIdx)+bt1_cellpointeroffset+i] = p[i];
        }
    } else {
        for(int i = 0; i < 4; i++){
            pb[bt1_headersize+(bt1_cellsize*bt1_cellsperpage)+i] = p[i];
        }
    }
}

/**
 * @brief Read a cell from pagebuffer into ret.
 */
void btbuffer_readcell(uint8_t* pb, uint8_t ret[bt1_cellsize], int cellIdx){
    for(int i = bt1_headersize+(bt1_cellsize*cellIdx); i < bt1_headersize+(bt1_cellsize*cellIdx)+bt1_cellsize; i++){
        ret[i-(bt1_headersize+(bt1_cellsize*cellIdx))] = pb[i];
    }
}

/**
 * @brief Write the header pointer on a pagebuffer
 */
void btbuffer_writeheader(uint8_t* pb, uint8_t h[4]){
    for(int i = 0; i < 4; i++){
        pb[i] = h[i];
    }
}

/**
 * @brief Read the header pointer on a pagebuffer
 */
void btbuffer_readheader(uint8_t* pb, uint8_t ret[4]){
    for(int i = 0; i < 4; i++){
        ret[i] = pb[i];
    }
}

uint32_t btbuffer_readheaderint(uint8_t* pb){
    uint8_t h[4];
    btbuffer_readheader(pb, h);
    return btree_pointertoint(h);
}

int btbuffer_celloffset(int cellIdx){
    return bt1_headersize+(cellIdx*bt1_cellsize);
}

/**
 * @brief Compare arrays
 * @return 1 if key1 is greater than key2, 0 if keys are equal, -1 if key1 is less than key2
 */
int btree_cellcmp(uint8_t cell1[bt1_cellsize], uint8_t cell2[bt1_cellsize]){
    return btree_keycmp(cell1,cell2);
}

/**
 * @brief Find the insertion IDX or the next page idx
 * @return -1 if down traversal necessary, 0 if insertion necessary, 1 if the keyExists. Ret is pageIdx on -1, insertionIdx on 0, cellIdx on 1
 */
int btree_findInsertionIdx(uint8_t pb[bt1_pagesize], uint8_t insCell[bt1_cellsize], uint32_t* ret){
    // if(bt1_debug){
    //     printf("FIIPRINT\n");
    //     btbuffer_printbuffer(pb);
    // }
    uint8_t emptyCell[bt1_cellsize];
    memset(emptyCell, 0, bt1_cellsize);
    int cmp_result;
    int empty_result;
    for(int i = 0; i < bt1_cellsperpage; i++){
        cmp_result = btree_cellcmp(insCell, &pb[btbuffer_celloffset(i)]);
        empty_result = btree_cellcmp(emptyCell, &pb[btbuffer_celloffset(i)]);
        // if(bt1_debug){
        //     printf("cmp_result=%i\n%u == %u\n",cmp_result,insCell[0] ,pb[btbuffer_celloffset(i)]);
        // }
        if(cmp_result == -1 || empty_result == 0){
            // if the cmp result is -1, check the before pointer
            uint32_t pointerIdx = btree_pointertoint(&pb[btbuffer_celloffset(i)+bt1_cellpointeroffset]);
            if(pointerIdx == 0){
                // Nothing there, insertion is necessary.
                *ret = i;
                return 0;
            } else {   
                // Traversal necessary.
                *ret = pointerIdx;
                return -1;
            }
        } else if (cmp_result == 0){
            // exists
            *ret = i;
            return 1;
        }
        // if the cmp result is 0, return 1 and the idx
        // if the cmp result is 1, something has gone wrong or you are looking at an empty cell.
    }
    // Check the after pointer aswell
    // If no after pointer exists, insert.
    uint32_t pointerIdx = btree_pointertoint(&pb[btbuffer_celloffset(bt1_cellsperpage)]);
    if(pointerIdx != 0){
        // pointer exists, traverse.
        *ret = pointerIdx;
        return -1;
    } else {
        // no pointer exists, insert (this will likely result in a split)
        *ret = bt1_cellsperpage;
        return 0;
    }
}

/**
 * @brief Traverse from root down to the location of a key or where it should be inserted
 * @return pageIdxRet returns the current page index, fiiResult the result of findInsertionIndex, and pb the current pageBuffer
 */
int btree_fileTraverse(FILE* treeFile, uint8_t insCell[bt1_cellsize], uint32_t* pageIdxRet, uint32_t* fiiResult, uint8_t pb[bt1_pagesize]){
    // Load ROOT
    fseek(treeFile, 0, SEEK_SET);
    fread(pb, sizeof(pb[0]), bt1_pagesize, treeFile);
    *pageIdxRet = 1;
    int fiiInd = btree_findInsertionIdx(pb, insCell, fiiResult);
    if(bt1_debug){
        printf("Starting-traverse, fiiInd = %i, fiiResult = %u\n",fiiInd, *fiiResult);
    }
    while(fiiInd == -1){
        fseek(treeFile, bt1_pagesize*((*fiiResult)-1), SEEK_SET);
        fread(pb, sizeof(pb[0]), bt1_pagesize, treeFile);
        *pageIdxRet = *fiiResult;
        fiiInd = btree_findInsertionIdx(pb, insCell, fiiResult);
        if(bt1_debug){
            printf("Mid-traverse, fiiInd = %i, fiiResult = %u\n",fiiInd, *fiiResult);
        }
    }
    return fiiInd;
}

int btree_memTraverse(uint8_t* mb, uint8_t insCell[bt1_cellsize], uint32_t* pageIdxRet, uint32_t* fiiResult, uint8_t pb[bt1_pagesize]){
    // Load ROOT
    for(int i = 0; i < bt1_pagesize; i++){
        pb[i] = mb[i];
    }
    *pageIdxRet = 1;
    int fiiInd = btree_findInsertionIdx(pb, insCell, fiiResult);
    while(fiiInd == -1){
        for(int i = 0; i < bt1_pagesize; i++){
            pb[i] = mb[(bt1_pagesize*((*fiiResult)-1))+i];
        }
        *pageIdxRet = *fiiResult;
        fiiInd = btree_findInsertionIdx(pb, insCell, fiiResult);
    }
    return fiiInd;
}

/**
 * @brief Check if a given page needs a split if a node is added
 * @return 1 if a split is necessary, otherwise 0 if is unnecessary.
 */
int btree_checkPageNeedsSplit(uint32_t idx, FILE* treeFile){
    // TODO: Modify this to work with the new cell focused, file agnostic approach.
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
 * @brief Return true if the last cell of the buffer's key is not empty, otherwise return false.
 */
int btbuffer_checkFull(uint8_t pb[bt1_pagesize]){
    uint8_t fakeCell[bt1_cellsize];
    memset(fakeCell, 0, bt1_cellsize);
    return 0 != btree_cellcmp(fakeCell, &pb[btbuffer_celloffset(29)]);
}

/**
 * @brief Insert the cell ic into the pb, potentially carrying over an after pointer into the end.
 * @return ret is set to the new buffer after insertion.
 */
void btree_insertCell(uint8_t pb[bt1_pagesize], uint8_t ret[bt1_pagesize], uint8_t ic[bt1_cellsize], int insIdx){
    // this is only called if there exists space to insert
    for(int i = 0; i < bt1_headersize; i++){
        ret[i] = pb[i];
    }
    for(int i = 0; i < bt1_cellsperpage+1; i++){
        if(i < insIdx){
            btbuffer_writecell(ret, &pb[btbuffer_celloffset(i)], i);
        } else if (i == insIdx){
            btbuffer_writecell(ret, ic, i);
        } else {
            btbuffer_writecell(ret, &pb[btbuffer_celloffset(i-1)], i);
        }
    }
    // REMEMBER ABOUT THE AFTER POINTER
    for(int i = 0; i < 4; i++){
        ret[btbuffer_celloffset(bt1_cellsperpage)] = pb[btbuffer_celloffset(bt1_cellsperpage-1)+bt1_cellpointeroffset];
    }
}

/**
 * @brief Insert insertCell ic into 31 size temporary node, split on the middle and fill out lb, rb, and pc with the left buffer, right buffer, and promoted cell.
 */
void btree_makeSplits(uint8_t pb[bt1_pagesize], uint8_t lb[bt1_pagesize], uint8_t rb[bt1_pagesize], uint8_t ic[bt1_cellsize], int insIdx, uint8_t pc[bt1_cellsize], bool isRoot){
    uint8_t plusbuf[bt1_pagesize+bt1_cellsize];
    //int cmp_result = btree_cellcmp(ic, &pb[btbuffer_celloffset(0)]);
    for(int i = 0; i < bt1_cellsperpage+1; i++){
        if(i < insIdx){
            btbuffer_writecell(plusbuf, &pb[btbuffer_celloffset(i)], i);
        } else if (i == insIdx){
            btbuffer_writecell(plusbuf, ic, i);
        } else {
            btbuffer_writecell(plusbuf, &pb[btbuffer_celloffset(i-1)], i);
        }
    }
    // 0-14 go to lb, 15 is pc, 16-30 are rb
    for(int i = 0; i < bt1_cellsperpage+1; i++){
        if(i < 15){
            btbuffer_writecell(lb, &plusbuf[btbuffer_celloffset(i)], i);
        } else if (i == 15){
            btbuffer_readcell(plusbuf, pc, i);
        } else {
            btbuffer_writecell(rb, &plusbuf[btbuffer_celloffset(i)], i-16);
        }
    }
    // Make sure to fill in the headers and after pointers.
    // pc's pointer becomes the after pointer of lb, pb's after pointer becomes the after pointer of rb
    int lapIdx = bt1_cellpointeroffset;
    int rapIdx = btbuffer_celloffset(30);
    btbuffer_writeapointer(lb, &pc[lapIdx], 15);
    btbuffer_writeapointer(rb, &pb[rapIdx], 15);
    // if isRoot, lb and rb get 0 0 0 1 as header, otherwise they inherit pb's header.
    if(isRoot){
        uint8_t header[4] = {0, 0, 0, 1};
        btbuffer_writeheader(lb, header);
        btbuffer_writeheader(rb, header);
    } else {
        uint8_t header[4];
        btbuffer_readheader(pb, header);
        btbuffer_writeheader(lb, header);
        btbuffer_writeheader(rb, header);
    }
}






// Load root into buffer, traverse tree until either key is found or no key exists.
// Return NULL if no such key exists, otherwise return pointer to value.
/**
 * @brief Finds the value of a given key if it exists in the btree, gives a pointer back to its temporary location in memory.
 * @return 1 if anything is found, otherwise 0. Return actual value to ret.
 */
int btree_findkey(FILE* treeFile, uint8_t key[64], uint8_t ret[64]){
    // (FILE* treeFile, uint8_t insCell[bt1_cellsize], uint32_t* pageIdxRet, uint32_t* fiiResult, uint32_t pb[bt1_pagesize])
    uint32_t fiiResult, pageIdx;
    uint8_t searchCell[bt1_cellsize], pageBuffer[bt1_pagesize];
    int traverseResult;
    memset(searchCell, 0, bt1_cellsize);
    for(int i = 0; i < 64; i++){
        searchCell[i] = key[i];
    }
    uint8_t* mb; // empty pointer to use in case of usememory == 1
    if(bt1_usememory){
        // TODO: load entire btree file into mb
        traverseResult = btree_memTraverse(mb, searchCell, &pageIdx, &fiiResult, pageBuffer);
    } else {
        traverseResult = btree_fileTraverse(treeFile, searchCell, &pageIdx, &fiiResult, pageBuffer);
    }
    // pageIdx is where we are currently, this is more useful for addvalue.
    // We care about if the value exists or not, this is the traverse result.
    // If the traverse result is 0 it doesn't exist, if 1 it does. fiiResult is the index if it does.
    if(bt1_debug){
        printf("Traverse results for finding: pageIdx = %u, fiiResult = %u, traverse result = %i.\n",pageIdx, fiiResult, traverseResult);
    }
    if(traverseResult){
        for(int i = 0; i < bt1_valsize; i++){
            ret[i] = pageBuffer[btbuffer_celloffset(fiiResult)+bt1_cellvalueoffeset+i];
        }
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief find which cell a pointer is at
 * @return -1 if none exist, otherwise the index.
 */
int btree_findPointerCell(uint8_t pb[bt1_pagesize], uint32_t pIdx){
    uint8_t pt[4];
    btree_inttopointer(pIdx, pt);
    for(int i = 0; i < bt1_cellsperpage; i++){
        bool isSame = true;
        for (int j = 0; j < 4; j++){
            isSame = isSame && pb[btbuffer_celloffset(i)+bt1_cellpointeroffset+j] == pt[j];
        }
        if(isSame){
            return i;
        }
    }
    return -1;
}

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, replace key value and return 1
// If no key is found, add the key and split if necessary, return 0
// If for some reason the key cannot be added, return -1
/**
 * @brief Add a value to the DB in the specified filedesc, prev will return the previous value if it exists, otherwise will become NULL
 * @return 1 if key replace, 0 if key added, -1 if key can't be added
 */
int btree_addvalue(FILE* treeFile, uint8_t key[64], uint8_t val[64], uint8_t prev[64]){
        // (FILE* treeFile, uint8_t insCell[bt1_cellsize], uint32_t* pageIdxRet, uint32_t* fiiResult, uint32_t pb[bt1_pagesize])
    uint32_t fiiResult, pageIdx;
    uint8_t insertCell[bt1_cellsize], pageBuffer[bt1_pagesize];
    int traverseResult;
    memset(insertCell, 0, bt1_cellsize);
    for(int i = 0; i < 128; i++){
        if(i<64){
            insertCell[i] = key[i];
        } else {
            insertCell[i] = val[i-64];
        }
    }
    uint8_t* mb; // empty pointer to use in case of usememory == 1
    if(bt1_usememory){
        // TODO: load entire btree file into mb
        traverseResult = btree_memTraverse(mb, insertCell, &pageIdx, &fiiResult, pageBuffer);
    } else {
        traverseResult = btree_fileTraverse(treeFile, insertCell, &pageIdx, &fiiResult, pageBuffer);
    }
    // pageIdx is where we are currently, this is more useful for addvalue.
    // We care about if the value exists or not, this is the traverse result.
    // If the traverse result is 0 it doesn't exist, if 1 it does. fiiResult is the index if it does.
    if(traverseResult){
        // Key exists and this is a lot simpler
        for(int i = 0; i < bt1_valsize; i++){
            prev[i] = pageBuffer[btbuffer_celloffset(fiiResult)+bt1_cellvalueoffeset+i];
        }
        btbuffer_writecell(pageBuffer, insertCell, fiiResult);
        if(bt1_usememory){
            for(int i = 0; i < bt1_pagesize; i++){
                mb[(bt1_pagesize*(pageIdx-1))+i] = pageBuffer[i];
            }
            // Write entire log back
        } else {
            fseek(treeFile, bt1_pagesize*(pageIdx-1), SEEK_SET);
            fwrite(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
        }
        return 1;
    } else if (fiiResult != bt1_cellsperpage && !btbuffer_checkFull(pageBuffer)) {
        // Insertion possible without splitting
        if(bt1_debug){
            printf("Inserting at %u.\n",pageIdx);
        }
        uint8_t retBuffer[bt1_pagesize];
        btree_insertCell(pageBuffer, retBuffer, insertCell, fiiResult);
        if(bt1_usememory){
            for(int i = 0; i < bt1_pagesize; i++){
                mb[(bt1_pagesize*(pageIdx-1))+i] = retBuffer[i];
            }
            // Write entire log back
        } else {
            fseek(treeFile, bt1_pagesize*(pageIdx-1), SEEK_SET);
            fwrite(retBuffer, sizeof(retBuffer[0]), bt1_pagesize, treeFile);
        }
        return 0;
    } 
    // It's split time, this could be an else but I want to save text room.
    uint8_t leftBuffer[bt1_pagesize], rightBuffer[bt1_pagesize], promotedCell[bt1_cellsize];
    //(uint8_t pb[bt1_pagesize], uint8_t lb[bt1_pagesize], uint8_t rb[bt1_pagesize], uint8_t ic[bt1_cellsize], int insIdx, uint8_t pc[bt1_cellsize], bool isRoot)
    memset(leftBuffer, 0, bt1_pagesize);
    memset(rightBuffer, 0, bt1_pagesize);
    int insIdx = fiiResult;
    while(btbuffer_checkFull(pageBuffer)){
        if(pageIdx == 1){
            // ROOT
            btree_makeSplits(pageBuffer, leftBuffer, rightBuffer, insertCell, insIdx, promotedCell, true);
            // left-side
            uint32_t newPageIdx = btree_findfreepage(treeFile);
            uint8_t newPagePtr[4];
            btree_inttopointer(newPageIdx, newPagePtr);
            for(int i = 0; i < 4; i++){
                promotedCell[bt1_cellpointeroffset+i] = newPagePtr[i];
            }
            fseek(treeFile, bt1_pagesize*(newPageIdx-1), SEEK_SET);
            fwrite(leftBuffer, sizeof(leftBuffer[0]), bt1_pagesize, treeFile);
            // right-side
            uint32_t newPageIdx2 = btree_findfreepage(treeFile);
            uint8_t newPagePtr2[4];
            btree_inttopointer(newPageIdx2, newPagePtr2);
            fseek(treeFile, bt1_pagesize*(newPageIdx2-1), SEEK_SET);
            fwrite(rightBuffer, sizeof(rightBuffer[0]), bt1_pagesize, treeFile);
            // btbuffer_writeapointer(uint8_t* pb, uint8_t p[4], int cellIdx)
            // btbuffer_writecell(uint8_t* pb, uint8_t cell[bt1_cellsize], int cellIdx)
            // Zero out pb, insert cell and write after pointer
            memset(pageBuffer, 0, bt1_pagesize);
            btbuffer_writecell(pageBuffer, promotedCell, 0);
            btbuffer_writeapointer(pageBuffer, newPagePtr2, 1);
            // write the new root.
            fseek(treeFile, 0, SEEK_SET);
            fwrite(pageBuffer, (sizeof(pageBuffer[0])), bt1_pagesize, treeFile);
            // rectify the children
            btree_rectifyChildrenParents(newPageIdx, treeFile);
            btree_rectifyChildrenParents(newPageIdx2, treeFile);
            if(bt1_debug){
                printf("ROOTRESULTS\nLEFT\n");
                btbuffer_printbuffer(leftBuffer);
                printf("RIGHT\n");
                btbuffer_printbuffer(rightBuffer);
                printf("PAGEBUFFER\n");
                btbuffer_printbuffer(pageBuffer);
            }
        } else {
            // NON-ROOT
            btree_makeSplits(pageBuffer, leftBuffer, rightBuffer, insertCell, insIdx, promotedCell, false);
            uint32_t newPageIdx = btree_findfreepage(treeFile);
            uint32_t parentIdx = btbuffer_readheaderint(pageBuffer);
            fseek(treeFile, bt1_pagesize*(parentIdx-1), SEEK_SET);
            fread(pageBuffer, sizeof(pageBuffer[0]), bt1_pagesize, treeFile);
            // fill in the insCell's pointer to the new idx.
            uint8_t newPagePtr[4];
            btree_inttopointer(newPageIdx, newPagePtr);
            for(int i = 0; i < 4; i++){
                promotedCell[bt1_cellpointeroffset+i] = newPagePtr[i];
            }
            insIdx = btree_findPointerCell(pageBuffer, pageIdx);
            // write left and right buffer
            fseek(treeFile, bt1_pagesize*(pageIdx-1), SEEK_SET);
            fwrite(rightBuffer, sizeof(rightBuffer[0]), bt1_pagesize, treeFile);
            fseek(treeFile,bt1_pagesize*(newPageIdx-1), SEEK_SET);
            fwrite(leftBuffer, sizeof(leftBuffer[0]), bt1_pagesize, treeFile);
            if(bt1_debug){
                printf("SPLITRESULTS\nLEFT\n");
                btbuffer_printbuffer(leftBuffer);
                printf("RIGHT\n");
                btbuffer_printbuffer(rightBuffer);
                printf("PB\n");
                btbuffer_printbuffer(pageBuffer);
            }
            // rectify children
            btree_rectifyChildrenParents(newPageIdx, treeFile);// use legacy method for now.
            if (btbuffer_checkFull(pageBuffer)){
                // still need to split again, set up for it
                for(int i = 0; i < bt1_cellsize; i++){
                    insertCell[i] = promotedCell[i];
                }
                pageIdx = parentIdx;
            } else {
                // insert and be done with it.
                uint8_t retBuffer[bt1_pagesize];
                btree_insertCell(pageBuffer, retBuffer, promotedCell, insIdx);
                // fill in in
                fseek(treeFile, bt1_pagesize*(parentIdx-1), SEEK_SET);
                fwrite(retBuffer, sizeof(retBuffer[0]), bt1_pagesize, treeFile);
            }
        }
    }
    return 0;
}

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, remove and merge as necessary, return 0.
// If no key exists, return -1.
/**
 * @brief Optional method to implement if time allows, to remove a value
 * @return
 */
int btree_removevalue(int* buffer, FILE* treeFile, char val[64]);

