#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../base/structure.h"
#include "../base/config.h"


__attribute__((hot, aligned(64))) int loadLimit(limit*** __restrict__ limits, limit* __restrict__ newLimit) {
    if (__builtin_expect(limits == NULL || *limits == NULL, 0)) {
        perror("limits is NULL!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect(strcmp((*limits)[i]->name, newLimit->name) == 0, 0)) {
            fprintf(stderr, "duplicated limits detected!\n");
            return -1;
        }
    }
    if (__builtin_expect(numberOfResLimits >= capacityOfResLimits, 0)) {
        int newCapacity = capacityOfResLimits + __INITIAL_SCALE_OF_RES_LIMIT__;
        limit** tmp = realloc((*limits), newCapacity);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to reallocate memory for limits!\n");
            return -1;
        }
        (*limits) = tmp;
        tmp = NULL;
        capacityOfResLimits = newCapacity;
    }
    (*limits)[++numberOfResLimits] = newLimit;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    return 0;
}