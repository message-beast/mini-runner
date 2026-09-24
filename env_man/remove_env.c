#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>


int removeEnv(env*** __restrict__ envs, char* __restrict__ __name, char* __restrict__ __key) {
    if (__builtin_expect(envs == NULL, 0)) {
        perror("envs is null!\n");
        return -1;
    }
    enum {FIND_ENV, FREE_ENV, FORMAT_ENV, DEL_ENV} state = FIND_ENV;
    env* foundEnv = NULL;
    int lastIndex = 0;
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 511) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i+512], 0, 3);
        }
        if (state == FIND_ENV) {
            env* current = (*envs)[i];
            if (__builtin_expect(strcmp(current->name, __name) == 0 && strcmp(current->key, __key) == 0, 0)) {
                foundEnv = current;
                state = FREE_ENV;
            }
        }
        if (state == FREE_ENV) {
            if (__builtin_expect(foundEnv != NULL, 1)) {
                free(foundEnv->name);
                free(foundEnv->key);
                free(foundEnv->value);
                free(foundEnv);
                foundEnv = NULL;
            }
            state = FORMAT_ENV;
        }
        if (state == FORMAT_ENV) {
            if (i == numberOfEnv - 1) {
                state = DEL_ENV;
            } else {
                (*envs)[i] = (*envs)[i + 1];
            }
        }
        if (state == DEL_ENV) {
            numberOfEnv --;
            __asm__ volatile (
                "sfence"
                :
                :
                : "memory"
            );
            if (__builtin_expect(numberOfEnv == 0, 0)) {
                free((*envs));
                (*envs) = NULL;
                envs = NULL;
                return 0;
            }
            return 0;
        }
    }
    fprintf(stderr, "can not find any env in %s with %s key\n", __name, __key);
    return -1;
}