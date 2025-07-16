
#include <stdint.h>
#include "btreeprojMEM.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

int main(void){
    // Time code from https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    printf("Current UStime = %lu\n", start.tv_sec * 1000000  + start.tv_usec);
    uint8_t addKey[64], addRet[64], addPrev[64];
    // Write 100 trials
    printf("100 Read Trial 1\n");
    FILE* fp = fopen("benchmark100-1.btree","r+");
    uint64_t sumTime = 0;
    uint64_t subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    printf("\nMicrosecond Average Final = %lu\n100 Read Trial 2\n",sumTime);
    fclose(fp);
    FILE* fp = fopen("\nbenchmark100-2.btree","r+");
    uint64_t size_mb;
    uint8_t* mb;
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to find random key: %u\r", i, btree_findkey(&mb, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100 Read Trial 3\n",sumTime);
    fp = fopen("benchmark100-3.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 100;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000 Read Trial 1\n",sumTime);
    // 1000 writes
    fp = fopen("benchmark1000-1.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n1000 Read Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark1000-2.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000 Read Trial 3\n",sumTime);
    fp = fopen("benchmark1000-3.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 1000;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n10000 Read Trial 1\n",sumTime);
    // 10,000 writes
    fp = fopen("benchmark10000-1.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n10000 Read Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark10000-2.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n10000 Read Trial 3\n",sumTime);
    fp = fopen("benchmark10000-3.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 10000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/10000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    }
    sumTime = subTotal / 10000;
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100000 Read Trial 1\n",sumTime);
    // 100,000
    fp = fopen("benchmark100000-1.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n100000 Read Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark100000-2.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n100000 Read Trial 3\n",sumTime);
    fp = fopen("benchmark100000-3.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 100000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/100000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/100000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/100000);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000000 Read Trial 1\n",sumTime);
    //1,000,000
    fp = fopen("benchmark1000000-1.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    free(mb);
    printf("\nMicrosecond Average Final = %lu\n1000000 Read Trial 2\n",sumTime);
    fclose(fp);
    fp = fopen("benchmark1000000-2.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n1000000 Read Trial 3\n",sumTime);
    fp = fopen("benchmark1000000-3.btree","r+");
    btree_readDB(fp, &mb, &size_mb);
    sumTime = 0;
    subTotal = 0;
    for(long long i = 0; i < 1000000; i++){
        for(int j = 0; j < 64; j++){
            addKey[j] = rand();
            memset(addRet, 0, 64);
        }
        gettimeofday(&start, NULL);
        printf("%lli/1000000: Result of attempting to find random key: %u\r", i, btree_findkey(fp, addKey, addRet));
        gettimeofday(&stop, NULL);
        subTotal = subTotal + ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
        if((i%10000)==0){
            sumTime = sumTime + (subTotal/1000000);
            subTotal = 0;
        }
    }
    sumTime = sumTime + (subTotal/1000000);
    free(mb);
    fclose(fp);
    printf("\nMicrosecond Average Final = %lu\n",sumTime);
}