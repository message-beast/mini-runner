#include "../base/structure.h"
#include <stdlib.h>
#include "../base/config.h"
void freeEnvs(env*** __envs) {
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin((*__envs) != NULL, 1)) {
            env* current = (*__envs);
            free(current->key);
            free(current->name);
            free(current->value);
            free(current);
            current = NULL;
        }
    }
}