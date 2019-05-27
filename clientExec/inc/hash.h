#include "entry.h"

#ifndef HASH_H
#define HASH_H

#define K 1<<29
#define HEX 16

/*
 * check if the weight value is the encoding of the service in the string s
 */
int try(char* s, int weight);

/*
 * iterate over all the provided services to find the correct one
 */
const char* findService(int service, char* services[], int numService);

/*
 * function that from the generated key recreates the service
 */
const char* decode(long long int key, struct entry_t e, char* serv[], int num);

/*
 * function that simulates the behavior of atoll, receiving an hexadecimal string number
 */
long long int atollHex(const char *);

#endif //HASH_H
