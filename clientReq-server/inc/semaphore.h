#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/*
 * definition of the union semun
 */
 union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

/* semOp is a support function to manipulate a semaphore's value
 * of a semaphore set. semid is a semaphore set identifier, sem_num is the
 * index of a semaphore in the set, sem_op is the operation performed on sem_num
 */
void semOp (int semid, unsigned short sem_num, short sem_op);

#endif //SEMAPHORE_H