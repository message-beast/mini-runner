#include <stdlib.h>
#include <stdio.h>
#include "../base/structure.h"

void freeArgs(envToArg** args) {
    if (__builtin_expect(args == NULL || *args == NULL, 0)) return;
    envToArg* __args = (*args);
    for (register int i = 0; i < __args->nums; ++i) {
        free(__args->args[i]);
    }
    free(__args->args);
    __args->args = NULL;
    free(__args);
    __args = NULL;
}


void freeArgsE(envToArg** args, int numOfArgs) {
    if (__builtin_expect(args == NULL || *args == NULL, 0)) {
        return;
    }
    envToArg* __args = (*args);
    for (register int i = 0; i < numOfArgs; ++i) {
        free(__args->args[i]);
    }
    free(__args->args);
    __args->args = NULL;
    free(__args);
    __args = NULL;
}