#include <stdio.h>
#include <string.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
int renameEnv(env*** __restrict__ envs, char* __restrict__ name, char* __restrict__ newName) {
    if (__builtin_expect(envs == NULL || *envs == NULL, 0)) {
        return -1;
    }
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i+256], 0, 3);
        }
        if (__builtin_expect(strcmp((*envs)[i]->name, name) == 0, 0)) {
            env* current = (*envs)[i];
            char* oldName = current->name;
            free(oldName);
            current->name = strdup(newName);
        }
    }
    return 0;
}