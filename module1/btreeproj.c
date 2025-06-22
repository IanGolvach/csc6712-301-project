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
 * @brief Finds the value of a given key if it exists in the btree, gives a pointer back to its temporary location in memory.
 * @return 1 if anything is found, otherwise 0. Return actual value to ret.
 */
int btree_findkey(int* buffer, int filedesc, char key[64], char ret[64]){
    // Load ROOT
    
    // Search ROOT for key OR next child pointer

    // Traverse down until LEAF is reached without success or until the KEY is found

    // If no key is found, return 0 and set ret to NULL

    // Else, if a key is found, copy the value to ret and return 1.
}

/**
 * @brief Add a value to the DB in the specified filedesc, prev will return the previous value if it exists, otherwise will become NULL
 * @return 1 if key replace, 0 if key added, -1 if key can't be added
 */
int btree_addvalue(int* buffer, int filedesc, char key[64], char val[64], char prev[64]){
    // Load ROOT
    
    // Search ROOT for key OR next child pointer

    // Traverse down until LEAF is reached without success or until the KEY is found

    // If no key is found, insert key on LEAF and split if necessary (see split pngs), set prev to NULL and return 0

      // SPLIT PROCEDURE
       // LEAF NODES: Split the leaf in half, first key of the second half is added to the parent with an after pointer to the others
        // Second half keys and parent idx need to stay in-memory after deletion of second half from OG and switching to new unoccupied page. Splitting key and indexes of both pages must stay in-memory when switching to parent.
       // NONLEAF NODES: Split the leaf in half, first key of the secon d half is addeed to the parent, original after pointer becomes the before pointer of new node.

    // Else, if a key is found replace the value at the key and set prev to the value, return 1
}

