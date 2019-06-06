#include "entry.h"

#ifndef HASH_H
#define HASH_H

/*
 * hashing constant values
 */
#define K 1<<29
#define HEX 16

/*
 * check if the weight value is the encoding of the service s
 */
int try(char* s, int w);

/*
 * iterate over all the provided services to find the correct one
 */
const char* findService(int weight);

/*
 * function that from the generated key recreates the service weight
 */
const char* decode(long long int key, struct entry_t e);

/*
 * function that simulates the behavior of atoll, receiving an hexadecimal string number
 * return -1 on error
 */
long long int atollHex(const char *);

#endif //HASH_H
