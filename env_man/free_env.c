#include "../base/structure.h"
#include <stdlib.h>
#include "../base/config.h"
void freeEnvs(env*** __envs) {
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*__envs)[i + 128], 0, 3);
        }
        if (__builtin_expect((*__envs) != NULL, 1)) {
            env* current = (*__envs)[i];
            free(current->key);
            free(current->name);
            free(current->value);
            free(current);
            current = NULL;
        }
    }
    free(*__envs);
    *__envs = NULL;
}