#include "entry.h"
#include <stdlib.h>

#ifndef KEYS_H
#define KEYS_H

/*
 * define max number of entries of the db and a macro to know if the db is full or not
 */
#define MAX_CLIENT 1000
#define DB_SPACE(x) MAX_CLIENT-(x)

/*
 * define constant values for the keys of the shared memory segments and the semaphore
 */
#define SEMDBKEY 2

#define SHMDBKEY 1
#define SHMLKEY 2

/*
 * defining constant values which will be used by the various programs
 */
char* serverFIFO = "/tmp/serverFIFO";
char* clientFIFO = "/tmp/clientFIFO.";
char* services[] = {"stampa\0", "salva\0", "invia\0"};
int numService = sizeof(services)/sizeof(char *);

#endif //KEYS_H