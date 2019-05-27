#ifndef HASH_H
#define HASH_H

/*
 * constant value for encoding the service
 */
#define K 1<<29

/*
 * function that returns the weight of the service specified in the string s
 */
int weight(const char* s);

/*
 * encoding of the key, given the asked service and the creation time
 */
long long int encode(const char* service, time_t creationTime);

#endif //HASH_H
