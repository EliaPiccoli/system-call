/// server.c
/// server of the service
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "errExit.h"
#include "response.h"
#include "request.h"
#include "semaphore.h"
#include "entry.h"
#include "hash.h"
#include "keys.h"

extern char* serverFIFO;
extern char* clientFIFO;
extern char* services[];
extern int numService;

int semdbid, shmid, shmdbid, cd, sd, fakesd;
struct entry_t* db;
int* length;
pid_t pid;

//handler to the SIGTERM signal
void sigHandler(int sig) {
    //only server will perform this routine
    if(pid > 0) {
        //send SIGTERM to keyManager
        if (kill(pid, SIGTERM) == -1)
            errExit("kill failed");
        //close all the file descriptors of the FIFOs, remove the serverFIFO, delete the semaphores
        //detach all the shared memory and deletes them
        if (sd != 0 && close(sd) == -1)
            errExit("close failed");
        if (fakesd != 0 && close(fakesd) == -1)
            errExit("close failed");
        if (unlink(serverFIFO) == -1)
            errExit("unlink failed");
        if (semctl(semdbid, 0, IPC_RMID, 0) == -1)
            errExit("semctl failed");
        if (shmdt(db) == -1)
            errExit("shmdt failed");
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
            errExit("shmctl failed");
        if (shmdt(length) == -1)
            errExit("shmdt failed");
        if (shmctl(shmdbid, IPC_RMID, NULL) == -1)
            errExit("shmctl failed");

        //wait termination of keyManager and deallocates his structure -> no zombie
        while(wait(NULL) == -1);

        printf("Server turning off\n");
        exit(0);
    } else {
        printf("keyManager turning off\n");
        exit(0);
    }
}

int main (int argc, char *argv[]) {
    printf("Starting the Server\n");

    //creating and setting the new sigset_t of the process
    sigset_t sigSet;
    if(sigfillset(&sigSet) == -1)
        errExit("sigfillset failed");
    if(sigdelset(&sigSet, SIGTERM) == -1)
        errExit("sigdelset failed");
    if(sigprocmask(SIG_SETMASK, &sigSet, NULL) == -1)
        errExit("sigprocmask failed");
    //adding handler to SIGTERM signal
    if(signal(SIGTERM, sigHandler) == SIG_ERR)
        errExit("signal failed");

    //creating the shared memory for the db
    //allocate memory for an array of struct entry_t
    shmid = shmget(SHMDBKEY, sizeof(struct entry_t)*MAX_CLIENT, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shmid == -1)
        errExit("shmget failed");

    //attach the shared memory to the server
    db = (struct entry_t *) shmat(shmid, NULL, 0);
    if(db == (struct entry_t *) NULL)
        errExit("shmat failed");

    //create shared memory which will use the server and keyManager to know how many entries there are in the db
    shmdbid = shmget(SHMLKEY, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shmdbid == -1)
        errExit("shmget failed");

    //attach the shared memory to the server
    length = (int *) shmat(shmdbid, NULL, 0);
    if(length == (int *) NULL)
        errExit("shmat failed");

    //set initial size of db
    *length = 0;

    //creating semaphore to access the shared memory db
    semdbid = semget(SEMDBKEY, 1, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if(semdbid == -1)
        errExit("semget failed");

    //set the initial value of the semaphore to 1
    union semun arg;
    arg.val = 1;
    if(semctl(semdbid, 0, SETVAL, arg) == -1)
        errExit("semctl failed");

    //create a children -> keyManager
    pid = fork();
    if(pid == 0) {
        //code that will be executed by KeyManager
        printf("Starting keyManager\n");

        //for the first five minutes will wait, since it won't find any key to delete
        //sleep(300);

        while(1) {
            //before accessing the db tries to get the semaphore (mutex)
            semOp(semdbid, 0, -1);
            //update shared memory deleting the old entries
            delEntry(db, length);
            //release the semaphore
            semOp(semdbid, 0, 1);
            //pauses 30s before clearing again the db
            sleep(30);
        }
    }

    //Server code
    printf("Server PID: %u, keyManager PID: %u\n", getpid(), pid);

    //create serverFIFO
    if(mkfifo(serverFIFO, S_IWUSR | S_IRUSR) == -1)
        errExit("mkfifo failed");

    //open in reading mode the severFIFO
    sd = open(serverFIFO, O_RDONLY);
    if(sd == -1)
        errExit("open failed");

    //open in writing mode th serverFIFO so it doesn't see EOF even if there are no clientReq processes
    fakesd = open(serverFIFO, O_WRONLY);
    if(fakesd == -1)
        errExit("open failed");

    //declaring the various structs that will be needed to read and write from/into the FIFO and to access the db
    struct request_t request;
    struct response_t response;
    struct entry_t entry;
    int knownService;
    char pathclientFIFO[25];

    while(1) {
        int bR = read(sd, &request, sizeof(struct request_t));
        if (bR == -1)
            break; //otherwise print wrong message
        else if (bR != sizeof(struct request_t))
            printf("Bad request\n");
        else {
            knownService = 0;
            printf("%s communicating with the Server\n", request.userId);
            //get the semaphore to access the shared memory segment containing the length od the db
            semOp(semdbid, 0, -1);
            //new request, increase the number of entry to check if there is space in the db for the new entry
            (*length)++;
            //check if there is space
            if (DB_SPACE(*length) < 0) {
                (*length)--;
                printf("[ERROR] Server db full.\n\tServer can't handle any more request!\n");
                semOp(semdbid, 0, 1);
            } else {
                semOp(semdbid, 0, 1);
                serviceToLowerCase(&request);
                printf(" ... Looking for service: %s ...\n", request.service);
                fflush(stdout);
                for (int i = 0; i < numService; i++) {
                    if (strcmp(request.service, services[i]) == 0) {
                        //we have found a knownService
                        knownService = 1;

                        //fill the struct entry with the info and updates the shared memory db
                        strcpy(entry.user, request.userId);
                        entry.timestamp = time(NULL);
                        //encode the key based on the service requested and the time of the request
                        entry.key = encode(request.service, entry.timestamp);
                        response.key = entry.key;
                        //before accessing the db tries to get the semaphore (mutex)
                        semOp(semdbid, 0, -1);
                        //add the entry to the shared memory db
                        addEntry(db, entry, length);
                        //release the semaphore
                        semOp(semdbid, 0, 1);
                    }
                }
                if (!knownService) {
                    response.key = encode(request.service, time(NULL));
                }
                strcpy(response.userId, request.userId);

                //recreating the path to the clientFIFO based on his pid
                sprintf(pathclientFIFO, "%s%d", clientFIFO, request.pid);

                //open in writing mode the clientFIFO
                cd = open(pathclientFIFO, O_WRONLY);
                if (cd == -1)
                    errExit("open failed");

                //write the response in the clientFIFO
                if (write(cd, &response, sizeof(struct response_t)) != sizeof(struct response_t))
                    errExit("write failed");

                //close file descriptor to clientFIFO
                if (close(cd) == -1)
                    errExit("close failed");
            }
        }
    }
}
