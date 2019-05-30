/// invia.c
/// send all the arguments to the msgque, of key arvg[1]
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/msg.h>
#include <errno.h>

#include "errExit.h"
#include "message.h"

int main (int argc, char *argv[]) {
    srand(time(NULL));

    //check command line arguments
    if(argc < 2) {
        printf("Usage: %s msgkey ...\n", *argv);
        return 1;
    }

    int msgKey = atoi(argv[1]);
    if(msgKey <= 0) {
        printf("message_key should be an integer greater than zero\n");
        return 1;
    }

    //get the id of the existing message_queue
    int msqid = msgget(msgKey, S_IWUSR);
    if(msqid == -1)
        errExit("msgget failed");

    //creating the message
    struct message_t msg;
    msg.mtype = rand()%100 + 1; //+1 because mtype must be greater then 0
    strcpy(msg.message, "");
    for(int i=0; i<argc-2; i++) {
        strncat(msg.message, *(argv + i + 2), STR_SIZE(*(argv + i + 2)));
        strcat(msg.message, "\n");
    }
    strcat(msg.message, "\0");

    //Print to terminal the message
    //printf("mtype: %ld\n", msg.mtype);
    //printf("message: %s\n", msg.message);

    //calculate msg size and them sends it to the msq
    size_t msgSize = sizeof(struct message_t) - sizeof(long);
    if(msgsnd(msqid, &msg, msgSize, IPC_NOWAIT) == -1) {
        if(errno == EAGAIN) {
            printf("message_queue full\n");
            return 1;
        }
        else
            errExit("msgsnd failed");
    }

    printf("Message successfully sent to the message_queue\n");

    return 0;
}
