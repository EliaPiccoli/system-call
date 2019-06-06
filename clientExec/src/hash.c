/// hash.c
/// decoding of the key
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "entry.h"

extern char* services[];
extern int numService;

int try(char* s, int w) {
    for(int i=0; i<strlen(s); i++)
        w ^= *(s + i);

    return w == K;
}

const char* findService(int weight) {
    for(int i=0; i<numService; i++)
        if(try(services[i], weight))
            return services[i];

    return NULL;
}

const char* decode(long long int key, struct entry_t e) {
    printf("Decoding key ...\n");

    long long int tmp = e.key^e.timestamp;
    int service = tmp/e.timestamp;

    return findService(service);
}

long long int atollHex(const char* s) {
    long long int num = 0;
    char c;

    for(int i=0; i<strlen(s); i++) {
        c = *(s+i);
        num *= HEX;
        if(c >= '0' && c <= '9')
            num += (c - '0');
        else if(c >= 'a' && c <= 'f')
            num += ((c - 'a') + 10);
        else return -1;
    }

    return num;
}