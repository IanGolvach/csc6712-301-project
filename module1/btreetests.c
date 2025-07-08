

#include <stdint.h>
#include "btreeproj.h"
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
    // //fclose(fp);
    // time_t startTime = time(NULL);
    // srand(time(NULL));
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    for(long long i = 0; i < 200000; i++){ // problem split at 514 and 46 and 5378 and 259
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
        if(i % 2 == 0){
            addKey[0] = 255 - (i % 256);
            addKey[1] = 255 - ((i / 256) % 256);
            addKey[2] = 255 - (i / (256*256)) % 256;

        } else {
            addKey[0] = i % 256;
            addKey[1] = (i / 256) % 256;
            addKey[2] = (i / (256*256)) % 256;
        }
        
        printf("%lli/200: Result of attempting to add a new key and val: %u\n", i, btree_addvalue(fp, addKey, addVal, addPrev));
        //printf("Result of attempting to find the new key: %u\n", btree_findkey(fp, addKey, addRet));
    }
    gettimeofday(&stop, NULL);
    memset(addKey, 1, 64);
    printf("Result of attempting to find the old key: %i\n", btree_findkey(fp, addKey, addRet));
    printf("100k insertions performed in %lu seconds\n",(stop.tv_sec - start.tv_sec));
    // printf("\nInserting 20000 random values took %.f seconds.", startTime - time(NULL));
    fclose(fp);
}