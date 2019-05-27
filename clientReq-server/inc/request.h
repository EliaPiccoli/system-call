#include <sys/types.h>

#ifndef REQUEST_H
#define REQUEST_H

/*
 * struct containing the info that the client sends to the Server to get a key to access the service
 */
struct request_t {
    char userId[100];
    char service[7];
    pid_t pid;
};

/*
 * function that receive a pointer to a struct request_t and parse the service to lowercase
 */
void serviceToLowerCase(struct request_t* r);

#endif //REQUEST_H
