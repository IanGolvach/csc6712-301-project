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
 * = 3968 bytes per page
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
#include <stdint.h>
#include <stdio.h>

#define bt1_pagesize 4096

#define bt1_cellsize 132

#define bt1_headersize 4

#define bt1_cellsperpage 30

#define bt1_cellkeyoffset 0

#define bt1_cellvalueoffeset 64

#define bt1_cellpointeroffset 128

#define bt1_tailsize 4

#define bt1_branching_factor 16 // No more than 2d cells per page, d is branch_factor - 1

#define bt1_keysize 64

#define bt1_valsize 64

#define bt1_debug 0

#define bt1_usememory 1

#define bt1_memorypages 4096*24

// Load root into buffer, traverse tree until either key is found or no key exists.
// Return NULL if no such key exists, otherwise return pointer to value.
/**
 * @brief Finds the value of a given key if it exists in the btree, gives a pointer back to its temporary location in memory.
 * @return 1 if anything is found, otherwise 0. Return actual value to ret.
 */
int btree_findkey(uint8_t** mb, uint8_t key[64], uint8_t ret[64]);

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, replace key value and return 1
// If no key is found, add the key and split if necessary, return 0
// If for some reason the key cannot be added, return -1
/**
 * @brief Add a value to the DB in the specified filedesc, prev will return the previous value if it exists, otherwise will become NULL
 * @return 1 if key replace, 0 if key added, -1 if key can't be added
 */
int btree_addvalue(uint8_t** mb, uint8_t key[64], uint8_t val[64], uint8_t prev[64], uint64_t* size_mb);

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
int btree_keycmp(uint8_t key1[64], uint8_t key2[64]);

/**
 * @brief Finds the earliest free index to write a page to
 * @return The integer index of the writable page
 */
uint32_t btree_findfreepage(FILE* treeFile);

/**
 * @brief Initializes the root for a new database in the given file.
 * @return 1 if successful, otherwise 0.
 */
int btree_createNewDB(FILE* treeFile);

/**
 * @brief Convert an array of 4 bytes to a 64 bit unsigned integer big endian.
 * @return the 64 bit pointer integer.
 */
uint32_t btree_pointertoint(uint8_t pointer[4]);

int btree_readDB(FILE* treeFile, uint8_t** mb, uint64_t* size_mb);

int btree_writeDB(FILE* treeFile, uint8_t** mb, uint64_t* size_mb);