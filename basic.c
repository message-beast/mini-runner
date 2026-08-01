#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "base/config.h"
int exit_process(int i) {
    if (i == 0) {
        exit(0);
    } else if (i < 0) {
        printf("error: %s\n", strerror(errno));
        exit(i);
    }
    abort();
}

#define exit_program(i) exit_process(i);


void cleanupStr(char** ptr) {
    if (*ptr) {
        free(*ptr);
    }
    free(ptr);
}


#define STR __attribute__((cleanup(cleanupStr)))
#if defined(DEBUG_MODE)
    #define DEBUG printf("###DEBUG##### %s: %i\n", __FILE__, __LINE__);
#else
    #define DEBUG 
#endif