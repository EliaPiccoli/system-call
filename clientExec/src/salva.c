/// salva.c
/// save in the file, argv[1], all the other arguments
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "errExit.h"

//sizeof of a string
#define STR_SIZE(X) sizeof(char)*strlen(X)

int main (int argc, char *argv[]) {
    //check command line input argument
    if(argc < 2) {
        printf("[Error] Service Salva failed to execute\n");
        printf("\tUsage: %s filename ...\n", *argv);
        return 1;
    }

    //open the file in wr-only
    //use append in case various processes want to send info to the same file
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
    if(fd == -1)
        errExit("open failed");

    //tmp buffer to build the string
    char* s = (char *) malloc(sizeof(char));
    //iterate over the elements in argv and writes them in the file
    for(int i=2; i<argc; i++) {
        s = realloc(s, STR_SIZE(*(argv + i))+1);
        strcat(s, *(argv + i));
        strcat(s, "\n");
        if(write(fd, s, STR_SIZE(s)) != STR_SIZE(s))
            errExit("write failed");
        strcpy(s, "\0");
     }
    free(s);

    if(close(fd) == -1)
        errExit("close failed");

    if(argc - 2 > 0)
        printf("All %d elements saved in file: %s\n", argc - 2, argv[1]);
    else printf("Nothing was added to the file: %s\n", argv[1]);

    return 0;
}
