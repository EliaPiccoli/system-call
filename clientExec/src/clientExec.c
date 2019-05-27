/// clientExec.c
/// execute, if allowed, the required functionality
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "errExit.h"
#include "entry.h"
#include "semaphore.h"
#include "hash.h"
#include "keys.h"

extern char* services[];
extern int numService;

//function that creates the argument vector for the exec syscall
char** createArgVector(int n, char** a) {
    char** argVector;
    //length = n - 3 + 1(name) + 1(NULL)
    int length = n - 1;
    argVector = (char **) malloc(sizeof(char *)*length);
    for(int i=1; i<length-1; i++)
        argVector[i] = *(a + i + 2);
    argVector[length-1] = (char *) NULL;

    return argVector;
}

int main (int argc, char *argv[]) {
    //check command line arguments
    if (argc < 3) {
        printf("Usage: %s user_identifier server_key ...\n", *argv);
        return 1;
    }

    //read the key
    long long int serverKey = atollHex(argv[2]);
    if (serverKey == -1) {
        printf("Invalid server key. Server generates only hexadecimal keys greater then zero.\n");
        exit(1);
    }

    //get the id of the semaphore to access the db
    int semdbid = semget(SEMDBKEY, 1, S_IRUSR | S_IWUSR);
    if (semdbid == -1)
        errExit("semget failed");

    //get the id of the shared memory db
    int shmid = shmget(SHMDBKEY, sizeof(struct entry_t)*MAX_CLIENT, S_IRUSR | S_IWUSR);
    if (shmid == -1)
        errExit("shmget failed");

    //attach the shared memory db segment
    struct entry_t *db = (struct entry_t *) shmat(shmid, NULL, 0);
    if (db == (struct entry_t *) NULL)
        errExit("shmat failed");

    //get the id of the shared memory which contains the number of entries of the db
    int shmdbid = shmget(SHMLKEY, sizeof(int), S_IRUSR | S_IWUSR);
    if (shmdbid == -1)
        errExit("shmget failed");

    //attach the shared memory segment which contains the length
    int *dblength = (int *) shmat(shmdbid, NULL, 0);
    if (dblength == (int *) NULL)
        errExit("shmat failed");

    //before searching into the server, tries to get the semaphore to access the db
    semOp(semdbid, 0, -1);

    //variable entry will contain the position into the db of the entry that correspond to the client
    //if there is no entry a negative value will be returned
    int entry = search(db, dblength, argv[1], serverKey);

    char service[7];
    //if (user, key) was a valid couple, decode the key to get the asked service
    if (entry >= 0) {
        strcpy(service, decode(serverKey, db[entry], services, numService));
        //deletes the key from the shared memory segment
        delEntry(db, dblength, entry);
    }

    //release the semaphore
    semOp(semdbid, 0, 1);

    //switch over entry to know if we have found an entry or we have errors
    switch (entry) {
        case -1: {
            printf("code [%d]: user_identifier and server_key don't appear in the db.\n", entry);
            printf("\t1. You might have asked for an unsupported service.\n");
            printf("\t2. Your key has already been used.\n");
            printf("\t3. Your key has expired.\n");
            printf("\t4. You are trying to access before requesting a key.\n");
            exit(1);
        }
        case -2: {
            printf("code [%d]: invalid key for user_identifier.\n", entry);
            exit(1);
        }
        case -3: {
            printf("code [%d]: invalid user_identifier for key.\n", entry);
            exit(1);
        }
        case -4: {
            printf("code [%d]: there is no combination (user_identifier, key).\n", entry);
            exit(1);
        }
        default: {
            printf("\nSoon starting service: %s ... \n\n", service);
            //create the argument vector for the exec call
            char **argV = createArgVector(argc, argv);
            char *s = (char *) malloc(sizeof(char)*9);
            strcpy(s, "./");
            if (strcmp(service, services[0]) == 0) {
                argV[0] = strcat(s, service);
                free(s);
                if (execvp("./stampa", argV) == -1)
                    errExit("execvp failed");
            } else if (strcmp(service, services[1]) == 0) {
                argV[0] = strcat(s, service);
                free(s);
                if (execvp("./salva", argV) == -1)
                    errExit("execvp failed");
            } else {
                argV[0] = strcat(s, service);
                free(s);
                if (execvp("./invia", argV) == -1)
                    errExit("execvp failed");
            }
        }
    }
}
