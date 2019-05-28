#ifndef KEYS_H
#define KEYS_H

/*
 * define max number of entries of the db
 */
#define MAX_CLIENT 300

/*
 * define constant values for the keys of the shared memory segment and the semaphore
 */
#define SEMDBKEY 2

#define SHMDBKEY 1
#define SHMLKEY 2

/*
 * define constant max length string which will contain the service name programm
 */
#define MAX_LEN 9

/*
 * defining constant values of the services provided
 */
char* services[] = {"stampa\0", "salva\0", "invia\0"};
int numService = sizeof(services)/sizeof(char *);

#endif //KEYS_H