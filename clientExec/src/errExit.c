/// errExit.c
/// error handling functions
#include "errExit.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void errExit(const char* msg) {
        perror(msg);
        exit(1);
}