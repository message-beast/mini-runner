#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <time.h>
#define true 1
#define false 0


__attribute__((hot)) int createJob(job*** __restrict__ jobs, char* __restrict__ jobName, char* __restrict__ runnableFile, __uint64_t secondsInterval) {
    if (__builtin_expect(jobs == NULL && *jobs == NULL, 0)) {
        job** tmp = malloc(__INITIAL_SCALE_SIZE_OF_JOBS__ * sizeof(job*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("memory allocation failed for jobs!\n");
            return -1;
        }
        (*jobs) = tmp;
        tmp = NULL;
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfJobs; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*jobs)[i + 128], 0, 3);
        }
        job* currentJob = (*jobs)[i];
        if (__builtin_expect(strcmp(currentJob->name, jobName) == 0, 0)) {
            fprintf(stderr, "\033[33mjob %s already exists!\033[0m\n", jobName);
            return -1;
        } else if (__builtin_expect(strcmp(currentJob->runnableFile, runnableFile) == 0, 0)) {
            fprintf(stderr, "\033[33m runnable file %s already exists in %s\033[0m\n", runnableFile, currentJob->name);
            return -1;
        }
    }

    job* newJob = malloc(sizeof(job));
    if (__builtin_expect(newJob == NULL, 0)) {
        perror("failed to allocate memory for new job!\n");
        return -1;
    }
    newJob->name = strdup(jobName);
    newJob->runnableFile = strdup(runnableFile);
    newJob->pid = 0;
    newJob->secondsInterval = secondsInterval;
    newJob->lastTimeRunned = time(NULL) + 1;
    if (__builtin_expect(numberOfJobs >= capacityOfJobs, 0)) {
        int newSize = numberOfJobs + __INITIAL_SCALE_SIZE_OF_JOBS__;
        job** tmp = realloc(*jobs, newSize * sizeof(job*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to re allocate memory for jobs!\n");
            return -1;
        }
        *jobs = tmp;
        tmp = NULL;
        capacityOfJobs = newSize;
    }


    job** _jobs = *jobs;
    _jobs[numberOfJobs] = newJob;
    if (__builtin_expect(_jobs[numberOfJobs] == NULL, 0)) {
        perror("failed to insert new job!\n");
        return -1;
    }
    printf("new job added:\n");
    printf("\033[33m|\033[31m-\033[32mname: \033[33m%s\n\033[33m|\033[31m-\033[32mrunnable-file: \033[33m%s\n\033[33m|\033[31m-\033[32mseconds-interval: \033[33m%li\033[0m\n", newJob->name, newJob->runnableFile, newJob->secondsInterval);
    numberOfJobs++;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    return 0;
}