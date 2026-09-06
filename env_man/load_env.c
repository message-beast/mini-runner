#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include <string.h>
__attribute__((hot, aligned(64))) int loadEnv(env*** __restrict__ __env, env* __restrict__ newEnv) {
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*__env)[i + 256], 0, 3);
        }
        env* current = (*__env)[i];
        if (__builtin_expect(strcmp(current->key, newEnv->key) == 0 && strcmp(current->name, newEnv->name) == 0, 0)) {
            perror("double keys detected!\n");
            return -1;
        }
    }
    if (__builtin_expect(numberOfEnv >= capacityOfEnv, 0)) {
        int newCapacity = sizeof(env*) * (capacityOfEnv + __INITIAL_SCALE_OF_ENV__);
        env** tmp = realloc((*__env), newCapacity);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("can't re allocate space for env!\n");
            return -1;
        }
        (*__env) = tmp;
        capacityOfEnv = newCapacity;
    }
    (*__env)[numberOfEnv] = newEnv;
    numberOfEnv++;
    __asm__ volatile(
        "sfence"
        :
        :
        : "memory"
    );
    return 0;
}