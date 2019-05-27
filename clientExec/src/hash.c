/// hash.c
/// decoding of the key
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "entry.h"

int try(char* s, int weight) {
    for(int i=0; i<strlen(s); i++)
        weight ^= *(s + i);

    return weight == K;
}

const char* findService(int service, char* services[], int numService) {
    for(int i=0; i<numService; i++)
        if(try(services[i], service))
            return services[i];

    return NULL;
}

const char* decode(long long int key, struct entry_t e, char* serv[], int num) {
    printf("Decoding key ...\n");

    long long int tmp = e.key^e.timestamp;
    int service = tmp/e.timestamp;

    return findService(service, serv, num);
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