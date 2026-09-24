#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "free_args.h"

static inline __attribute__((always_inline, hot, aligned(64))) char* parseEnvToString(char* __restrict__ key, char* __restrict__ value) {
    size_t size = snprintf(NULL, 0, "%s=%s", key, value);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate environment parsed string length!\n");
        return NULL;
    }
    char* result = malloc(size + 1);
    if (__builtin_expect(result == NULL, 0)) {
        perror("memory allocation failed for env kv pair!\n");
        return NULL;
    }
    snprintf(result, size + 1, "%s=%s", key, value);
    return result;
}


__attribute__((hot, aligned(64))) envToArg* getArgs(env*** __restrict__ envs, char* __restrict__ name) {
    char** results = malloc(sizeof(char*) * __INITIAL_SCALE_OF_ENV__);
    if (__builtin_expect(results == NULL, 0)) {
        perror("memory allocation for extraction of env failed!\n");
        return NULL;
    }
    envToArg* arg = malloc(sizeof(envToArg));
    if (__builtin_expect(arg == NULL, 0)) {
        perror("failed to allocate memory for arg!\n");
        free(results);
        return NULL;
    }
    int found = 0;
    arg->capacity = __INITIAL_SCALE_OF_ENV__;
    arg->args = results;
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i + 256], 0, 3);
        }
        if (__builtin_expect(strcmp((*envs)[i]->name, name) == 0, 0)) {
            env* current = (*envs)[i];
            char* kvPair = parseEnvToString(current->key, current->value);
            if (__builtin_expect(kvPair == NULL, 0)) {
                perror("parsing env to kv pair failed\n");
                freeArgsE(&arg, found);    
                return NULL;
            }
            if (__builtin_expect(found >= arg->capacity, 0)) {
                int newSize = sizeof(char*) * (arg->capacity + __INITIAL_SCALE_OF_ENV__);
                char** tmp = realloc(arg->args, newSize);
                if (__builtin_expect(tmp == NULL, 0)) {
                    perror("reallocation for env args failed!\n");
                    freeArgsE(&arg, found);
                    return NULL;
                }
                arg->args = tmp;
                arg->capacity = newSize;
                tmp = NULL;
            }
            arg->args[found] = kvPair;
            found++;
        }
    }
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    arg->args[found+1] = NULL;
    arg->nums = found;
    return arg;   
}