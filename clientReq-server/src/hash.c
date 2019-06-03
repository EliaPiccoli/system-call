/// hash.c
/// encode service
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"

int weight(const char* s) {
    int counter = K;
    for(int i=0; i<strlen(s); i++)
        counter ^= *(s+i);

    return counter;
}

long long int encode(const char* service, time_t creationTime) {
    printf("Encoding key ...\n");

    long long int key = (weight(service)*((long long int) creationTime))^creationTime;
    printf("Generated key: %llu -> %0llx\n", key, key);

    //since timestamp is used to encode the key, and timestamp is the factor which makes all the keys unique
    //from creating one key going to the next one, we set a second of pause to maintain the uniqueness of the keys
    sleep(1);

    return key;
}