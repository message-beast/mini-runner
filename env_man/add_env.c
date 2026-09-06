#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
int addEnv(env*** __restrict__ envs, char* __restrict__ __name, char* __restrict__ __key, char* __restrict__ __value) {

    if (__builtin_expect(envs == NULL, 0)) {
        perror("envs is null!\n");
        return -1;
    }
    for (register int i = 0 ; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i + 256], 0, 3);
        }
        env* current = (*envs)[i];
        if (__builtin_expect(strcmp(current->name, __name) == 0 && strcmp(current->key, __key) == 0, 0)) {
            fprintf(stderr, "duplication detected. %s has also %s env key!\n", __name, __key);
            return 0;
        }
        current = NULL;
    }
    env* newEnv = malloc(sizeof(env));
    if (__builtin_expect(newEnv == NULL, 0)) {
        perror("failed to allocate memory for new env!\n");
        return -1;
    }
    char* name = strdup(__name);
    char* key = strdup(__key);
    char* value = strdup(__value);
    newEnv->name = name;
    newEnv->key = key;
    newEnv->value = value;
    if (__builtin_expect(numberOfEnv >= capacityOfEnv, 0)) {
        int newSize = sizeof(env*) * (capacityOfEnv + __INITIAL_SCALE_OF_ENV__);
        env** tmp = realloc((*envs), newSize);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to re-allocate memory for envs!\n");
            free(newEnv);
            free(name);
            free(key);
            free(value);
            return -1;
        }
        (*envs) = tmp;
        capacityOfEnv = newSize;
    }
    (*envs)[numberOfEnv] = newEnv;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    if (__builtin_expect((*envs)[numberOfEnv] != NULL, 1)) {
        numberOfEnv++;
        return 0;
    }
    return -1;
}