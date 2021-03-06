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
        if (kill(pid, SIGTERM) == -1)
            errExit("kill failed");
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

    sigset_t sigSet;
    if(sigfillset(&sigSet) == -1)
        errExit("sigfillset failed");
    if(sigdelset(&sigSet, SIGTERM) == -1)
        errExit("sigdelset failed");
    if(sigprocmask(SIG_SETMASK, &sigSet, NULL) == -1)
        errExit("sigprocmask failed");
    if(signal(SIGTERM, sigHandler) == SIG_ERR)
        errExit("signal failed");

    shmid = shmget(SHMDBKEY, sizeof(struct entry_t)*MAX_CLIENT, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shmid == -1)
        errExit("shmget failed");

    db = (struct entry_t *) shmat(shmid, NULL, 0);
    if(db == (struct entry_t *) NULL)
        errExit("shmat failed");

    shmdbid = shmget(SHMLKEY, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shmdbid == -1)
        errExit("shmget failed");

    length = (int *) shmat(shmdbid, NULL, 0);
    if(length == (int *) NULL)
        errExit("shmat failed");

    //set initial size
    *length = 0;

    semdbid = semget(SEMDBKEY, 1, IPC_CREAT | IPC_EXCL | S_IWUSR | S_IRUSR);
    if(semdbid == -1)
        errExit("semget failed");

    union semun arg;
    arg.val = 1;
    if(semctl(semdbid, 0, SETVAL, arg) == -1)
        errExit("semctl failed");

    //create a children -> keyManager
    pid = fork();
    if(pid == -1)
        errExit("fork failed");
    else if(pid == 0) {
        //code that will be executed by KeyManager
        printf("Starting keyManager\n");

        while(1) {
            sleep(30);
            semOp(semdbid, 0, -1);
            delEntry(db, length);
            semOp(semdbid, 0, 1);
        }
    }

    //Server code
    printf("Server PID: %u, keyManager PID: %u\n", getpid(), pid);

    if(mkfifo(serverFIFO, S_IWUSR | S_IRUSR) == -1)
        errExit("mkfifo failed");

    sd = open(serverFIFO, O_RDONLY);
    if(sd == -1)
        errExit("open failed");

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
            break;
        else if (bR != sizeof(struct request_t))
            printf("Bad request\n");
        else {
            knownService = 0;
            printf("%s communicating with the Server\n", request.userId);

            semOp(semdbid, 0, -1);

            //new request, increase the number of entry to check if there is space in the db for the new entry
            (*length)++;
            //check if there is space
            if (DB_SPACE(*length) < 0) {
                (*length)--;
                printf("[ERROR] Server db full.\n\tServer can't handle any more request!\n");
                response.key = -1;
            } else {
                serviceToLowerCase(&request);
                printf(" ... Looking for service: %s ...\n", request.service);
                for (int i = 0; i < numService; i++) {
                    if (strcmp(request.service, services[i]) == 0) {
                        printf(" ... service found ... ");
                        //we have found a knownService
                        knownService = 1;

                        //fill the struct entry with the info and updates the shared memory db
                        strcpy(entry.user, request.userId);
                        entry.timestamp = time(NULL);
                        //encode the key based on the service requested and the time of the request
                        entry.key = encode(request.service, entry.timestamp);
                        response.key = entry.key;
                        //add the entry to the shared memory db
                        addEntry(db, entry, length);
                    }
                }
                if (!knownService) {
		    (*length)--;
                    printf(" ... service not found ... ");
                    response.key = encode(request.service, time(NULL));
                }
                strcpy(response.userId, request.userId);
            }

            semOp(semdbid, 0, 1);

            //recreating the path to the clientFIFO based on his pid
            sprintf(pathclientFIFO, "%s%d", clientFIFO, request.pid);

            cd = open(pathclientFIFO, O_WRONLY);
            if (cd == -1)
                errExit("open failed");

            if (write(cd, &response, sizeof(struct response_t)) != sizeof(struct response_t))
                errExit("write failed");

            if (close(cd) == -1)
                errExit("close failed");
        }
    }
}
