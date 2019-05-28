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
            if (exit_value == -3)
                exit_value = -4;
            else exit_value = -2;
        }
        //wrong user
        else if(shm[i].key == serverKey) {
            if(exit_value == -2)
                exit_value = -4;
            else exit_value = -3;
        }
    }

    return exit_value;
}

void delEntry(struct entry_t* shm, int* length, int pos) {
    //replace the pos-th value with the last one
    shm[pos] = shm[*length-1];

    //decrement the number of entries int the db
    (*length)--;

    printf("Key removed from db\n");
}