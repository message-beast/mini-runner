#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include <string.h>


__attribute__((hot, aligned(64))) envGroup* getEnvs(env*** __restrict__ envs, char* __restrict__ name) {
    envGroup* group = malloc(sizeof(envGroup));
    if (__builtin_expect(group == NULL, 0)) {
        perror("failed to allocate memory for group!\n");
        return NULL;
    }
    group->envs = malloc(sizeof(env*) * __INITIAL_SCALE_OF_ENV_GROUP__);
    if (__builtin_expect(group->envs == NULL, 0)) {
        perror("failed to allocate memory for group envs!\n");
        free(group);
        return NULL;
    }
    group->capacpty = __INITIAL_SCALE_OF_ENV_GROUP__;
    group->nums = 0;
    int found = 0;
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i + 256], 0, 3);
        }
        if (__builtin_expect(strcmp((*envs)[i]->name, name) == 0, 0)) {
            if (__builtin_expect(found >= group->capacpty, 0)) {
                int newSize = group->capacpty + __INITIAL_SCALE_OF_ENV_GROUP__;
                env** tmp = realloc(group->envs, newSize);
                if (__builtin_expect(tmp == NULL, 0)) {
                    perror("failed to re-allocate memory for group envs!\n");
                    free(group);
                    free(group->envs);
                    return NULL;
                }
                group->envs = tmp;
                tmp = NULL;
            }
            group->envs[++found] = (*envs)[i];
        }
    }
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    group->nums = found;
    return group;
}




void showEnv(env*** __restrict__ envs, char* __restrict__ name, char* __restrict__ key) {
    if (__builtin_expect(envs == NULL || *envs == NULL, 0)) {
        fprintf(stderr, "\033[33mthere is no envs!\n\033[0m");
        return;
    }
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i + 256], 0, 3);
        }
        env* current = (*envs)[i];
        if (__builtin_expect(strcmp(current->name, name) == 0 && strcmp(current->key, key) == 0, 0)) {
            printf("\033[33m[%s] \033[31m%s\033[0m=\033[32m%s\n\033[0m", name, key, current->value);
            return;
        }
    }
    fprintf(stderr, "can not find any env in %s with a key of %s\n", name, key);
    return;
}