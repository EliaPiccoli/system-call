#ifndef KEYS_H
#define KEYS_H

/*
 * define max number of entries of the db
 */
#define MAX_CLIENT 1000

/*
 * define constant values for the keys of the shared memory segment and the semaphore
 */
#define SEMDBKEY 2

#define SHMDBKEY 1
#define SHMLKEY 2

/*
 * defining constant values of the services provided
 */
char* services[] = {"stampa\0", "salva\0", "invia\0"};
int numService = sizeof(services)/sizeof(char *);

#endif //KEYS_H