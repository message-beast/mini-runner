#include <stdlib.h>
#include <stdio.h>
#include "../../base/structure.h"
#define false 0
#define true 1
void freeMrnExecs(mrnExec** __mrn_exec) {
    if (__builtin_expect(*__mrn_exec == NULL, 0)) {
        return;
    }
    mrnExec* current = (*__mrn_exec);
    while (current != NULL) {
        if (__builtin_expect(current->commands == NULL, 0)) {
            mrnExec* before = current;
            current = current->next;
            free(before);
            before = NULL;
            continue;
        }
        for (register int j = 0; j < current->numbers; ++j) {
            free(current->commands[j]);
        }
        free(current->commands);
        mrnExec* before = current;
        current = current->next;
        free(before);
        before = NULL;
    }
    *__mrn_exec = NULL;
}