#include <time.h>

#ifndef ENTRY_H
#define ENTRY_H

/*
 * struct containing the entry in our shared memory
 */
struct entry_t {
    char user[100];
    long long int key;
    time_t timestamp;
};

/*
 * function that search into db if there is an entry which contains (user, serverKey)
 * return values:
 *      (>= 0) if there is an entry (position of the entry)
 *      -1 NO user - NO key
 *      -2 User - NO key
 *      -3 NO User - key
 *      -4 User with other keys/key associated to other user
 */
int search(struct entry_t* shm, int* length, char* user, long long int serverKey);

/*
 * deletes from the shared memory db the pos-th entry
 */
void delEntry(struct entry_t* shm, int* length, int pos);

#endif //ENTRY_H
