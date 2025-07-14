
#include <stdint.h>
#include "btreeprojMEM.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(void){
    // Time code from https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    printf("Current UStime = %lu\n", start.tv_sec * 1000000  + start.tv_usec);
    uint8_t addKey[64], addVal[64], addPrev[64];
    // Write 100 trials
    printf("100 Write Trial 1\n");
    FILE* fp = fopen("benchmark100-1.btree","w+");
    int createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    uint64_t size_mb;
    uint8_t* mb;
    btree_readDB(fp, &mb, &size_mb);
    uint64_t sumTime = 0;
    uint64_t subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    printf("\nMicrosecond Average Final = %lu\n100 Write Trial 2\n",sumTime);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    fp = fopen("benchmark100-2.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100 Write Trial 3\n",sumTime);
    fp = fopen("benchmark100-3.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000 Write Trial 1\n",sumTime);
    // 1000 writes
    fp = fopen("benchmark1000-1.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    printf("\nMicrosecond Average Final = %lu\n1000 Write Trial 2\n",sumTime);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    fp = fopen("benchmark1000-2.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000 Write Trial 3\n",sumTime);
    fp = fopen("benchmark1000-3.btree","w+");
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n10000 Write Trial 1\n",sumTime);
    // 10,000 writes
    fp = fopen("benchmark10000-1.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n10000 Write Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark10000-2.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n10000 Write Trial 3\n",sumTime);
    fp = fopen("benchmark10000-3.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100000 Write Trial 1\n",sumTime);
    // 100,000
    fp = fopen("benchmark100000-1.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n100000 Write Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark100000-2.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100000 Write Trial 3\n",sumTime);
    fp = fopen("benchmark100000-3.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
       subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
       if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000000 Write Trial 1\n",sumTime);
    //1,000,000
    fp = fopen("benchmark1000000-1.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n1000000 Write Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark1000000-2.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000000 Write Trial 3\n",sumTime);
    fp = fopen("benchmark1000000-3.btree","w+");
    createResult = btree_createNewDB(fp);
    printf("Result of attempting to create a new db: %u\n", createResult);
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            addVal[j] = rand();
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to add a new key and val: %u\r", i, btree_addvalue(&mb, addKey, addVal, addPrev, &size_mb));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    btree_writeDB(fp, &mb, &size_mb);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n",sumTime);
}