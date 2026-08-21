#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include "daemon_load.h"
#include "../basic.h"
#include "../sync/check.h"
#include <string.h>
static inline __attribute__((always_inline, hot)) char* giveString(char* string, int startingIndex, int endingIndex) {
    int length = endingIndex - startingIndex;
    char* finalString = malloc(length + 1);
    if (__builtin_expect(finalString == NULL, 0)) {
        perror("can not allocate memory for the string");
        return NULL;
    }
    memcpy(finalString, string + startingIndex, length);
    finalString[length] = '\0';
    return finalString;
}



__attribute__((hot)) int loadJobsDaemon(job*** jobs) {
    if (__builtin_expect(jobs == NULL, 0)) {
        job** tmp = malloc(sizeof(job*) * __INITIAL_SCALE_SIZE_OF_JOBS__);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to allocte memory for jobs!\n");
            return -1;
        }
        (*jobs) = tmp;
        tmp = NULL;
    }
    while(!isJobFree());
    int jobsFileFd = open("data/jobs", O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(jobsFileFd == -1, 0)) {
        perror("failed to open data/jobs file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(jobsFileFd, &st) != 0, 0)) {
        perror("fstat failed on jobs file!\n");
        return -1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, jobsFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap faled on jobs file!\n");
        return -1;
    }
    enum State {FIND_NAME, FIND_RUNNABLE, FIND_TIME, FIND_INTERVAL, FIND_PID} state = FIND_NAME;
    int lastIndex = 0;
    char* name = NULL;
    char* runnableFile = NULL;
    time_t lastTimeRunned = 0;
    __uint64_t interval = 0;
    __uint32_t pid = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < st.st_size; ++i) {
        if (__builtin_expect((i & 31) == 0 || i == 0, 0)) {
            __builtin_prefetch(&data[i + 32], 0, 3);
        }
        switch(state) {
            case FIND_NAME:
                if (__builtin_expect(data[i] == '^', 0)) {
                    char* nameTmp = giveString(data, lastIndex, i);
                    if (__builtin_expect(nameTmp == NULL, 0)) {
                        perror("can not parse name!\n");
                        exit_program(1)
                    }
                    name = nameTmp;
                    nameTmp = NULL;
                    state = FIND_RUNNABLE;
                    lastIndex = i + 1;
                }
                break;
            case FIND_RUNNABLE:
                if (__builtin_expect(data[i] == '#', 0)) {
                    char* runnableFileTmp = giveString(data, lastIndex, i);
                    if (__builtin_expect(runnableFileTmp == NULL, 0)) {
                        perror("can not parse runnable file!\n");
                        return -1;
                    }
                    runnableFile = runnableFileTmp;
                    runnableFileTmp = NULL;
                    state = FIND_TIME;
                    lastIndex = i + 1;
                }
                break;
            case FIND_TIME:
                if (__builtin_expect(data[i] == '$', 0)) {
                    char* lastTimeRunnedBuff = giveString(data, lastIndex, i);
                    if (__builtin_expect(lastTimeRunnedBuff == NULL, 0)) {
                        perror("failed to parse last time runned buff!\n");
                        return -1;
                    }
                    __uint64_t lastTimeRunnedNum = atol(lastTimeRunnedBuff);
                    if (__builtin_expect(lastTimeRunnedBuff == 0, 0)) {
                        perror("failed to parse lasttimerunned buffer to lasttime runned!\n");
                        return -1;
                    }
                    lastTimeRunned = lastTimeRunnedNum;
                    free(lastTimeRunnedBuff);
                    lastTimeRunnedBuff = NULL;
                    state = FIND_INTERVAL;
                    lastIndex = i+ 1;
                }               
                break;
            case FIND_INTERVAL:
                if (__builtin_expect(data[i] == '@', 0)) {
                    char* intervalBuff = giveString(data, lastIndex, i);
                    if (__builtin_expect(intervalBuff == NULL, 0)) {
                        perror("memory allocation for interval buffer failed!\n");
                        return -1;
                    }
                    __uint64_t intervalNum = atol(intervalBuff);
                    if (__builtin_expect(intervalNum == 0, 0)) {
                        perror("failed to parse interval buff!\n");
                        return -1;
                    }
                    interval = intervalNum;
                    free(intervalBuff);
                    intervalBuff = NULL;
                    state = FIND_PID;
                    lastIndex = i + 1;
                }
                break;
            case FIND_PID:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    char* pidBuff = giveString(data, lastIndex, i);
                    if (__builtin_expect(pidBuff == NULL, 0)) {
                        perror("failed to allocate memory for pid buffer!\n");
                        return -1;
                    }
                    __uint32_t pidNum = atoi(pidBuff);
                    pid = pidNum;
                    free(pidBuff);
                    pidBuff = NULL;
                    job* newJob = malloc(sizeof(job));
                    if (__builtin_expect(newJob == NULL, 0)) {
                        perror("failed to allocate space for newJob!\n");
                        return -1;
                    }
                    newJob->name = name;
                    newJob->runnableFile = runnableFile;
                    newJob->lastTimeRunned = lastTimeRunned;
                    newJob->secondsInterval = interval;
                    newJob->pid = pid;
                    if (__builtin_expect(loadJobDaemon(jobs, newJob) != 0, 0)) {
                        return -1;
                    }
                    state = FIND_NAME;
                    lastIndex = i + 1;
                }
                break;
        }
    }
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );
    return 0;
}