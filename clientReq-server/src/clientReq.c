/// clientReq.c
/// ask the Server the key to access at the required service
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "errExit.h"
#include "request.h"
#include "response.h"
#include "keys.h"

extern char* serverFIFO;
extern char* clientFIFO;
extern char* services[];
extern int numService;

void welcome() {
    printf("Hi and welcome to Client Request!\n");
    printf("Ask for a service and the server will provide you the key to access.\n");
    printf("The provided services are:\n");
    for(int i=0; i<numService; i++)
        printf("\t> %s\n", services[i]);
    printf("\n\n");
}

int main (int argc, char *argv[]) {
    welcome();

    //get the userID and the service and save them in the struct request_t
    struct request_t request;
    printf("Insert your UserIdentifier [NO space]:\t");
    scanf("%99s", request.userId);
    printf("Insert the Service you want to access:\t");
    scanf("%6s", request.service);

    //create the client fifo using the base clientFIFO path and adding his pid
    char pathclientFIFO[25];
    pid_t myPid = getpid();
    request.pid = myPid;
    sprintf(pathclientFIFO, "%s%d", clientFIFO, myPid);

    //create clientFIFO
    if(mkfifo(pathclientFIFO, S_IRUSR | S_IWUSR) == -1)
        errExit("mkfifo failed");

    //open in writing mode the serverFIFO
    int sd = open(serverFIFO, O_WRONLY);
    if(sd == -1)
        errExit("open failed");

    //write the message to the serverFIFO
    if(write(sd, &request, sizeof(struct request_t)) != sizeof(struct request_t))
        errExit("write failed");

    printf("\nSending request to server waiting for response...\n");

    //open in reading mode the clientFIFO
    int cd = open(pathclientFIFO, O_RDONLY);
    if(cd == -1)
        errExit("open failed");

    //read the response from clientFIFO
    struct response_t response;
    int bR = read(cd, &response, sizeof(struct response_t));
    if(bR == -1)
        errExit("read failed");
    else if(bR != sizeof(struct response_t))
        printf("Bad response\n");
    else {
        printf("\n");
        if(response.key > 0) {
            printf("Key:\t%0llx\n", response.key);
            printf("UserId:\t%s\n", response.userId);
        } else printf("Server can't handle the request!\nMost likely the db is full, try again in a few seconds.\n");
    }

    //close client file descriptor and remove the clientFIFO
    if(close(cd) == -1)
        errExit("close failed");
    if(unlink(pathclientFIFO) == -1)
        errExit("unlink failed");

    return 0;
}