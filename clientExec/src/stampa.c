/// stampa.c
/// print, as strings, all the elements in the array argv
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    if(argc == 1)
        printf("Nothing to print!\n");
    else {
        for (int i = 1; i < argc; i++)
            printf("%s\n", *(argv + i));
    }

    return 0;
}
