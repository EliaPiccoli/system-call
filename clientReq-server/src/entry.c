/// entry.c
/// management of the shared memory db
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "errExit.h"
#include "entry.h"

void swap(struct entry_t* shm, int x, int y) {
    struct entry_t tmp = *(shm + x);
    *(shm + x) = *(shm + y);
    *(shm + y) = tmp;
}

void addEntry(struct entry_t* shm, struct entry_t e, int* l) {
    strcpy(shm[*l-1].user, e.user);
    shm[*l-1].key = e.key;
    shm[*l-1].timestamp = e.timestamp;

    printdb(shm, l);
}

void delEntry(struct entry_t* shm, int* l) {
    time_t now = time(NULL);
    printf("\nkeyManager cleaning the db ...");
    fflush(stdout);
    for(int i=0; i<*l; i++){
        if(now - shm[i].timestamp >= MAX_TIME) {
            //swapping current entry with the last one
            swap(shm, i, *l-1);
            //last entry is not considered anymore
            (*l)--;
            //restart from current position in order to check if the swapped entry is valid
            i--;
        }
    }

    printdb(shm, l);
}

void printdb(struct entry_t *shm, int* length) {
    printf("\nPrinting the db\n");
    for(int i=0; i<*length; i++) {
        printf("-------------------\n");
        printf("User: %s\n", shm[i].user);
        printf("Key: %0llx\n", shm[i].key);
        printf("Timestamp: %d\n", (int)shm[i].timestamp);
    }
    printf("\n");
}