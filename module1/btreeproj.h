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
 * Splits?
 *
 *
 * Merges?
 * 
 * 
 * */

// Load root into buffer, traverse tree until either key is found or no key exists.
// Return NULL if no such key exists, otherwise return pointer to value.
int* btree_findkey(int* buffer, int filedesc);

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, replace key value and return 1
// If no key is found, add the key and split if necessary, return 0
// If for some reason the key cannot be added, return -1
int btree_addvalue(int* buffer, int filedesc);

// Load root into buffer, traverse tree until either key is found or no key exists.
// If key is found, remove and merge as necessary, return 0.
// If no key exists, return -1.
int btree_removevalue(int* buffer, int filedesc);

