#include <time.h>

#ifndef ENTRY_H
#define ENTRY_H

/*
 * maximum number of second to consider a key valid
 */
#define MAX_TIME 300

/*
 * struct containing the entry in our shared memory
 */
struct entry_t {
    char user[100];
    long long int key;
    time_t timestamp;
};

/*
 * add a new entry at the end of the shared memory db
 */
void addEntry(struct entry_t* shm, struct entry_t e, int* l);

/*
 * remove all the entries in the db with a timestamp older than MAX_TIME seconds
 */
void delEntry(struct entry_t* shm, int* l);

/*
 * swap two entries in the db
 */
void swap(struct entry_t* shm, int x, int y);

/*
 * prints all the entries inside the shared memory db
 */
void printdb(struct entry_t *shm, int* length);

#endif //ENTRY_H
