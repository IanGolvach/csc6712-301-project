extern "C" {
    #include "../module1/btreeproj.h"
}

#include <map>


// SELECT will be used for multiplexing https://man7.org/linux/man-pages/man2/select.2.html
// Order in which read items are done does not matter.
// First, check new connections for transactions
// drop locked transactions, initiate new ones.
// Put new transactions in the read fd_set for later.
// Check new transations from read_fds
// Write responses back to write_fds
// loop back to select

// Transactions have their own write log ( a map ) in order to buffer input
// STD::MAP https://en.cppreference.com/w/cpp/container/map.html
// new on transaction start
// Insert on a WRITE
// contains on a READ, otherwise check KVS
// flush to disk on end.
// delete on transaction end

// WRITE-AHEAD-LOG is written and saved constantly to deal with possible mid closures.
// Notes in the online file but
// LOG 
// <START T_x>
// <WRITE T_x, K_i, V1_i, V2_i>
// <ABORT T_x>
// <COMMIT T_x>
// Read forward, put writes (V2) to the buffer
// Undo back (V1) any that are not committed or aborted
// Flush
// TO MAKE IT ALL EASY:
// have T_x, K_i, V1_i, V2_i be fixed width
// K_i, V1_i,V2_i are all 64 uint8s
// T_x is a uint64
// "START "
// "WRITE "
// "ABORT "
// "COMMIT"
// Read in six char bytes to determine log type
// Based on type, either read in 1 64 bit unsigned int, or 1 6 4bit unsigned int and 3 groups of 64 8 bit unsigned ints.

// Transactions can run concurrent

// Transactions hold locks on all keys they are affecting. 
// STD::MAP https://en.cppreference.com/w/cpp/container/map.html
// Keep track of locks with a map.
// insert to place a lock
// contains to check for a lock
// erase to remove lock

// On commit, write to the KVS

// BUFFER FORMAT
// MAP
// Key, NEW VAL, OLD VAL
// MAP ON KEY

// Switch to CPP in order to make use of std::map
