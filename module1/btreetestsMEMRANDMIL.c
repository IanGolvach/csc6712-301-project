#include <stdint.h>
#include "btreeprojMEM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

int main(void){
    printf("We running with it!\n");
    // Lets create a basic file to work with.
    FILE* fp = fopen("test.tmp","w+");
    printf("File opened.\n");
    int createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    // get the buffer and size
    uint64_t size_mb;
    uint8_t* mb;
    btree_readDB(fp, &mb, &size_mb);
    uint8_t addKey[64], addVal[64], addPrev[64], addRet[64];
    memset(addKey, 1, 64);
    memset(addVal, 2, 64);
    memset(addPrev, 0, 64);
    memset(addRet, 0, 64);
    printf("Result of attempting to add a new key and val: %u\n", btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
    printf("Result of attempting to find the new key: %u\n", btree_findkey(&mb, addKey, addRet));
    bool isCorrect = true;
    for(int i = 0; i < 64; i++){
        isCorrect = isCorrect && addVal[i] == addRet[i];
    }
    if(isCorrect){
        printf("The value was returned correctly!\n");
    }
    printf("Result of attempting to readd a new key and val: %u\n", btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
    isCorrect = true;
    for(int i = 0; i < 64; i++){
        isCorrect = isCorrect && addVal[i] == addPrev[i];
    }
    if(isCorrect){
        printf("The prev value was returned correctly!\n");
    }
    // //fclose(fp);
    // time_t startTime = time(NULL);
    // srand(time(NULL));
    long num_tests = 1000000;
    uint seed = 123456;
    struct timeval stop, start;
    srand(seed);
    gettimeofday(&start, NULL);
    for(long long i = 0; i < num_tests; i++){ // problem split at 514 and 46 and 5378 and 259
        //FILE* fp = fopen("test.tmp","r+");
        // if(i%2==0){
        //     for(long long j = 0; j < 64; j++){
        //         //printf("Here we go...\n");
        //         addKey[j] = 255-i;
        //         addVal[j] = 255-i;
        //     }
        // }
        // else{
        //     for(long long j = 0; j < 64; j++){
        //         //printf("Here we go...\n");
        //         addKey[j] = i;
        //         addVal[j] = i;
        //     }
        // }
        memset(addKey, 5, 64);
        memset(addVal, 5, 64);
        // addKey[0] = i % 256;
        // addKey[1] = i / 256;
        for (int j = 0; j < 64; j++){
            addKey[j] = abs(rand())%256;
        }
        if(i==0){
            printf("starting first key = %u", addKey[0]);
        }
        addVal[0] = i % 256;
        addVal[1] = (i / 256) % 256;
        addVal[2] = (i / (256*256)) % 256;
        
        printf("%lli/200: Result of attempting to add a new key and val: %u, current size of buffer %lu\n", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb), size_mb);
        //printf("Result of attempting to find the new key: %u\n", btree_findkey(fp, addKey, addRet));
    }
    gettimeofday(&stop, NULL);
    memset(addKey, 1, 64);
    printf("Result of attempting to find the old key: %i\n", btree_findkey(&mb, addKey, addRet));
    printf("100000 insertions performed in %lu seconds\n",(stop.tv_sec - start.tv_sec));
    srand(seed);
    for(long long i = 0; i < num_tests; i++){ // problem split at 514 and 46 and 5378 and 259
        //FILE* fp = fopen("test.tmp","r+");
        // if(i%2==0){
        //     for(long long j = 0; j < 64; j++){
        //         //printf("Here we go...\n");
        //         addKey[j] = 255-i;
        //         addVal[j] = 255-i;
        //     }
        // }
        // else{
        //     for(long long j = 0; j < 64; j++){
        //         //printf("Here we go...\n");
        //         addKey[j] = i;
        //         addVal[j] = i;
        //     }
        // }
        memset(addKey, 5, 64);
        memset(addVal, 5, 64);
        // addKey[0] = i % 256;
        // addKey[1] = i / 256;
        for (int j = 0; j < 64; j++){
            addKey[j] = abs(rand())%256;
        }
        if(i==0){
            printf("starting first key = %u", addKey[0]);
        }
        addVal[0] = i % 256;
        addVal[1] = (i / 256) % 256;
        addVal[2] = (i / (256*256)) % 256;
        int findResult = btree_findkey(&mb, addKey, addRet);
        printf("%lli/200: Result of attempting to find an old key and val: %u, current size of buffer %lu\n", i, findResult, size_mb);
        if(findResult!=1){
            printf("Failed to find a key we inserted previously!\n");
            printf("Key = ");
            for(int i = 0; i < 64; i++){
                printf("%u ", addKey[i]);
            }
            printf("\n");
            btree_writeDB(fp, &mb, &size_mb);
            free(mb);
            fclose(fp);
            return 0;
        }
        isCorrect = true;
        for(int i = 0; i < 64; i++){
            isCorrect = isCorrect && addVal[i] == addRet[i];
        }
        if(isCorrect){
            printf("The value was returned correctly!\n");
        } else {
            printf("Value was not returned correctly.\n");
            return 0;
        }
        //printf("Result of attempting to find the new key: %u\n", btree_findkey(fp, addKey, addRet));
    }
    // printf("\nInserting 20000 random values took %.f seconds.", startTime - time(NULL));
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
}