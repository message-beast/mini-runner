#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../base/structure.h"
#include "../base/config.h"
int removeLimit(limit*** __restrict__ limits, char* __restrict__ name) {
    enum {FIND_LIMIT, FREE_LIMIT, FMT_LIMIT, DEL_LIMIT} state = FIND_LIMIT;
    limit* foundLimit = NULL;
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) __builtin_prefetch(&(*limits)[i+64], 0, 3);
        if (state == FIND_LIMIT) {
            if (__builtin_expect(strcmp((*limits)[i]->name, name) == 0, 0)) {
                foundLimit = (*limits)[i];
                state = FREE_LIMIT;
            }
        }
        if (state == FREE_LIMIT) {
            free(foundLimit->name);
            free(foundLimit);
            state = FMT_LIMIT;
        }
        if (state == FMT_LIMIT) {
            if (__builtin_expect(i == numberOfResLimits - 1, 0)) {
                state = DEL_LIMIT;
            } else {
                (*limits)[i] = (*limits)[i + 1];
            }
        }
        if (state == DEL_LIMIT) {
            numberOfResLimits--;
            if (__builtin_expect(numberOfResLimits == 0, 0)) {
                free((*limits));
                *limits = NULL;
            }
            return 0;
        }        
    }
    return 0;
}