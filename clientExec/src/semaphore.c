/// semaphore.c
/// operation on a semaphore
#include <sys/sem.h>

#include "semaphore.h"
#include "errExit.h"

void semOp (int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop = {sem_num, sem_op, 0};

    if (semop(semid, &sop, 1) == -1)
        errExit("semop failed");
}