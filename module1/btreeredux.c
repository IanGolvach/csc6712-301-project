#include "btreeproj.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
            pb[bt1_headersize+(bt1_cellsize*cellidx)+bt1_cellpointeroffset+i] = p[i];
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

int btbuffer_celloffset(int cellIdx){
    return bt1_header+(cellIdx*bt1_cellsize);
}

/**
 * @brief Compare arrays
 * @return 1 if key1 is greater than key2, 0 if keys are equal, -1 if key1 is less than key2
 */
void btree_cellcmp(uint8_t cell1[bt1_cellsize], uint8_t cell2[bt1_cellsize]){
    return btree_keycmp(&cell1[bt1_headersize],&cell2[bt1_headersize]);
}

/**
 * @brief Insert the cell ic into the pb, potentially carrying over an after pointer into the end.
 */
void btree_insertCell(uint8_t pb[bt1_pagesize], uint8_t ret[bt1_pagesize], uint8_t ic[bt1_cellsize], int insIdx){
    
}

/**
 * @brief Insert insertCell ic into 31 size temporary node, split on the middle and fill out lb, rb, and pc with the left buffer, right buffer, and promoted cell.
 */
void btree_makeSplits(uint8_t pb[bt1_pagesize], uint8_t lb[bt1_pagesize], uint8_t rb[bt1_pagesize], uint8_t ic[bt1_cellsize], int insIdx, uint8_t pc[bt1_cellsize], bool isRoot){
    uint8_t plusbuf[bt1_pagesize+bt1_cellsize];
    //int cmp_result = btree_cellcmp(ic, &pb[btbuffer_celloffset(0)]);
    for(int i = 0; i < bt1_cellsize+1; i++){
        if(i < insIdx){
            btbuff_writecell(plusbuf, &pb[btbuffer_celloffset(i)], i);
        } else if (i == insIdx){
            btbuff_writecell(plusbuf, ic, i);
        } else {
            btbuff_writecell(plusbuf, &pb[btbuffer_celloffset(i-1)], i);
        }
    }
    // 0-14 go to lb, 15 is pc, 16-30 are rb
    for(int i = 0; i < bt1_cellsize+1; i++){
        if(i < 15){
            btbuff_writecell(lb, &plusbuf[btbuffer_celloffset(i)], i);
        } else if (i == 15){
            btbuff_readcell(plusbuf, pc, i);
        } else {
            btbuff_writecell(rb, &plusbuf[btbuffer_celloffset(i)], i-16);
        }
    }
    // Make sure to fill in the headers and after pointers.
    // pc's pointer becomes the after pointer of lb, pb's after pointer becomes the after pointer of rb
    int lapIdx = bt1_cellpointeroffset;
    int rapIdx = btbuffer_celloffset(30);
    btbuffer_writeapointer(lb, &pc[lapIdx], int 15);
    btbuffer_writeapointer(rb, &pb[rapIdx], int 15);
    // if isRoot, lb and rb get 0 0 0 1 as header, otherwise they inherit pb's header.
    if(isRoot){
        uint8_t header[4] = {0, 0, 0, 1};
        btbuffer_writeheader(lb, header);
        btbuffer_writeheader(rb, header);
    } else {
        uint8_t header[4];
        btbuffer_readerheader(pb, header);
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
int btree_findkey(FILE* treeFile, uint8_t key[64], uint8_t ret[64]);

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, replace key value and return 1
// If no key is found, add the key and split if necessary, return 0
// If for some reason the key cannot be added, return -1
/**
 * @brief Add a value to the DB in the specified filedesc, prev will return the previous value if it exists, otherwise will become NULL
 * @return 1 if key replace, 0 if key added, -1 if key can't be added
 */
int btree_addvalue(FILE* treeFile, uint8_t key[64], uint8_t val[64], uint8_t prev[64]);

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, remove and merge as necessary, return 0.
// If no key exists, return -1.
/**
 * @brief Optional method to implement if time allows, to remove a value
 * @return
 */
int btree_removevalue(int* buffer, FILE* treeFile, char val[64]);

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
    for(int i = 0; i < 4; i++){
        printf("%u ", pointer[i]);
    }
    printf("\n");
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

