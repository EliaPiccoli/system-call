/// entry.c
/// management of the shared memory db
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/shm.h>

#include "errExit.h"
#include "entry.h"

int search(struct entry_t* shm, int* length, char* user, long long int serverKey) {
    int exit_value = -1;

    for(int i=0; i<*length; i++) {
        //check the entry
        if (strcmp(shm[i].user, user) == 0 && shm[i].key == serverKey)
            return i;
        //wrong key
        else if(strcmp(shm[i].user, user) == 0) {
            exit_value = -2;
        }
        //key found but with another user, since the keys are unique can't find any other entry
        else if(shm[i].key == serverKey) {
            if(exit_value == -2)
                return -4;
            else return -3;
        }
    }

    return exit_value;
}

void delEntry(struct entry_t* shm, int* length, int pos) {
    //replace the pos-th value with the last one
    shm[pos] = shm[*length-1];

    //decrement the number of entries int the db
    (*length)--;

    printf("Entry removed from db\n");
}