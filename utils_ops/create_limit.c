#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include "../arch/mem_barrier.h"

__attribute__((hot, aligned(64))) int createLimit(limit*** __restrict__ limits, char* __restrict__ name, __uint64_t cpuLimit, __uint128_t memLimit) {
    if (__builtin_expect(limits == NULL || *limits == NULL, 0)) {
        perror("limit is null!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect((i & 511) == 0 || i == 0, 0)) __builtin_prefetch(&(*limits)[i+512], 0, 3);
        if (__builtin_expect(strcmp(name, (*limits)[i]->name) == 0, 0)) {
            limit* currentLimit = (*limits)[i];
            currentLimit->cpu = cpuLimit;
            currentLimit->memory = memLimit;
            return 0;
        }
    }
    limit* newLimit = malloc(sizeof(limit));
    if (__builtin_expect(newLimit == NULL, 0)) {
        perror("failed to allocate memory for new limit!\n");
        return -1;
    }
    newLimit->name = strdup(name);
    newLimit->memory = memLimit;
    newLimit->cpu = cpuLimit;
    if (__builtin_expect(numberOfResLimits >= capacityOfResLimits, 0)) {
        int newCapacity = capacityOfResLimits + __INITIAL_SCALE_OF_RES_LIMIT__;
        limit** tmp = realloc((*limits), sizeof(limit*) * newCapacity);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to reallocate memory for limits!\n");
        }
        (*limits) = tmp;
        tmp = NULL;
        capacityOfResLimits = newCapacity;
    }
    (*limits)[numberOfResLimits] = newLimit;
    numberOfResLimits++;
    __MFENCE__
    return 0;
}