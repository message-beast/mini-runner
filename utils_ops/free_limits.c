#include <stdio.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"

void freeLimits(limit*** limits) {
    if (__builtin_expect(limits == NULL || *limits == NULL, 0)) {
        return;
    }
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) __builtin_prefetch(&(*limits)[i+128], 0, 3);
        limit* currentLimit = (*limits)[i];
        free(currentLimit->name);
        free(currentLimit);
    }
    free(*limits);
    *limits = NULL;
    return;
}