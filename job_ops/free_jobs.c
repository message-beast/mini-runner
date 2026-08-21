#include <stdio.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"


__attribute__((hot)) void freeJobs(job*** jobs) {
    if (__builtin_expect(jobs != NULL && *jobs != NULL, 1)) {
        for (register int i = 0; i < numberOfJobs; ++i) {
            if (__builtin_expect((*jobs)[i] != NULL, 1)) {
                job* currentJob = (*jobs)[i];
                free(currentJob->name);
                currentJob->name = NULL;
                free(currentJob->runnableFile);
                currentJob->runnableFile = NULL;
                free(currentJob);
                currentJob = NULL;
            }
        }
        free(*jobs);
        *jobs = NULL;
        jobs = NULL;
    }
}