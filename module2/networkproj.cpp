extern "C" {
    #include "../module1/btreeproj.h"
}


#include <map>

// Communication with client needs to be ROBUST

// ACK at the beginning of messages that are not asking for a RESEND
// All values sent MUST have a CRC code appended at the end to ensure the durability of messages sent
// Be aware of how you want to handle, protocols, etc. 
// Doesn't need to be extremely robust but it needs to be there
// Necessary methods
//   PUT (k,v) -> {v_old, NULL, key_locked, transaction_expired}
//   GET (k) ->   {v, null, key-locked failure, transaction-expired failure}:
//   CONTAINS ->  {bool, key-locked failure, transaction-expired failure}
//   SHUTDOWN: New connections are no longer accepted, running connections run until dead
//   START TRANSACTION (k_1...) -> {exp_time, lock failure}
//   COMMIT -> {success, no-writes-failure message, transaction-expired failure}
//   ABORT -> {ACK}

// READ looks at the memory buffer, GET doesn't.
// 
// You can do things outside of the transcaction?
// Have to check if the keys are in the transaction (a linked list?) before a read or write.
// Whatever is okay apparently so who knows

// USEFUL ASCII CHARACTERS
// ACKNOWLEDGE = d06, x06
// NEG. ACK. =   d21, x15
// ENQUIRY =     d05, x05
// NULL =        d00, x00
// CANCEL =      d24, x18
// START TEXT =  d02, x02
// END TEXT  =   d03, x03
// END TRANS. =  d04, x04

// ACK at start of messages to let client/server know previous value laden message was recieved correctly
// Neg. ACK if there is a conflict with the CRCs
// Begin transaction start with START TEXT
// Commits with END TEXT
// Aborts with CANCEL
// CONTAINS with ENQ
// Shutdown with END TRANSMISSION


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
// <S|T_x>
// <W|T_x|K_i|V1_i|V2_i>
// <A|T_x>
// <C|T_x>
// Read forward, put writes (V2) to the buffer
// Undo back (V1) any that are not committed or aborted
// Flush
// TO MAKE IT ALL EASY:
// have T_x, K_i, V1_i, V2_i be fixed width
// K_i, V1_i,V2_i are all 64 uint8s
// T_x is a uint64
// "S" START
// "W" WRITE
// "A" ABORT
// "C" COMMIT
// Read in one char byte to determine type
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
