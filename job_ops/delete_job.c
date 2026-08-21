#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"


__attribute__((hot)) int removeJob(job*** __restrict__ jobs, char* __restrict__ jobName) {
    if (__builtin_expect(jobs == NULL || *jobs == NULL && numberOfJobs == 0, 0)) {
        return 0;
    }
    enum {FIND_JOB, FREE_JOB, FMT_JOB, DEL_JOB} state = FIND_JOB;
    for (register int i = 0; i < numberOfJobs; ++i) {
        if (state == FIND_JOB) {
            if (__builtin_expect(strcmp((*jobs)[i]->name, jobName) == 0, 0)) {
                state = FREE_JOB;
            }
        }
        if (state == FREE_JOB) {
            job* currentJob = (*jobs)[i];
            printf("job to delete:\n\033[31m|\033[33m-\033[32mname: \033[33m%s\n\033[31m|\033[33m-\033[32mrunnable-file: \033[33m%s\n\033[31m|\033[33m-\033[32mseconds-interval: \033[33m%li\033[0m\n", currentJob->name, currentJob->runnableFile, currentJob->secondsInterval);
            free(currentJob->name);
            free(currentJob->runnableFile);
            currentJob->name = NULL;
            currentJob->runnableFile = NULL;
            free(currentJob);
            currentJob = NULL;
            state = FMT_JOB;
        }
        if (state == FMT_JOB) {
            if (i == numberOfJobs - 1) {
                state = DEL_JOB;
            } else {
                (*jobs)[i] = (*jobs)[i + 1];
            }
        }
        if (state == DEL_JOB) {
            numberOfJobs--;
            if (__builtin_expect(numberOfJobs == 0, 0)) {
                free(*jobs);
                *jobs = NULL;
            }
            return 0;
        }

    }
    if (__builtin_expect(state == FIND_JOB, 0)) {
        fprintf(stderr, "\033[31mcan not find any job with name \033[33m\"%s\"\033[0m\n", jobName);
        return -1;
    }
    return 0;
}