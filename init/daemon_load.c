#pragma optimiez("O3")
#include <stdio.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>


__attribute__((hot)) int loadJobDaemon(job*** jobs, job* newJOb) {
    if (__builtin_expect(numberOfJobsDaemon >= capacityOfJobsDaemon, 0)) {
        int newSize = capacityOfJobsDaemon + __INITIAL_SCALE_SIZE_OF_JOBS__;
        job** tmp = realloc((*jobs), sizeof(job*) * newSize);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to reallocate memory for job!\n");
            return -1;
        }
        (*jobs) = tmp;
        capacityOfJobsDaemon = newSize;
        tmp = NULL;
    }
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfJobsDaemon; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*jobs)[i + 64], 0, 3);
        }
        if (__builtin_expect(strcmp((*jobs)[i]->name, newJOb->name) == 0 || strcmp((*jobs)[i]->runnableFile, newJOb->runnableFile) == 0, 0)) {
            fprintf(stderr, "failed to load duplicated projects! with name or runnable files!\n");
            return -1;
        }
    }
    (*jobs)[numberOfJobsDaemon] = newJOb;
    if (__builtin_expect((*jobs)[numberOfJobsDaemon] == NULL, 0)) {
        perror("failed to load new project!\n");
        return -1;
    }
    numberOfJobsDaemon++;
    return 0;
}