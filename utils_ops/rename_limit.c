#define _POSIX_C_SOUCRE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
int renameLimit(limit*** __restrict__ limits, char* __restrict__ name, char* __restrict__ newName) {
    if (__builtin_expect(limits == NULL || *limits == NULL, 0)) {
        return -1;
    }
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect((i & 511) == 0 || i == 0, 0)) __builtin_prefetch(&(*limits)[i+512], 0, 3);
        if (__builtin_expect(strcmp((*limits)[i]->name, name) == 0, 0)) {
            limit* currentLimit = (*limits)[i];
            char* beforeName = currentLimit->name;
            free(beforeName);
            currentLimit->name = strdup(newName);
            beforeName = NULL;
            return 0;
        }
    }
    fprintf(stderr, "can not find any limit with name %s\n");
    return 0;
}