#include <stdio.h>
#include <string.h>

#include "request.h"

void serviceToLowerCase(struct request_t* r) {
    for(int i=0; i<strlen(r->service); i++) {
        char c = r->service[i];
        if(c >= 'A' && c <= 'Z')
            r->service[i] = c + 32;
    }
}