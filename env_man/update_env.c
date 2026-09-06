#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
int updateEnv(env*** __restrict__ envs, char* __restrict__ __name, char* __restrict__ __key, char* __restrict__ __value) {
    if (__builtin_expect(envs == NULL, 0)) {
        perror("envs is null!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i + 256], 0, 3);
        }
        env* current = (*envs)[i];
        if (__builtin_expect(strcmp(current->name, __name) == 0 && strcmp(current->key, __key) == 0, 0)) {
            free(current->value);
            current->value = strdup(__value);
            return 0;
        }
    }
    fprintf(stderr, "failed to find %s env key in %s", __name, __key);
    return -1;
}