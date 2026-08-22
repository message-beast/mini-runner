#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
#include "../basic.h"
#include <stdarg.h>
#include "../sync/sync.h"
#include "../sync/check.h"
#define true 1
#define false 0

static inline __attribute__((always_inline, hot)) int writeData(char* dataTobeWritten) {
    int jobsFileFd = open("data/jobs", O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(jobsFileFd == -1, 0)) {
        perror("failed to open jobs file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(jobsFileFd, &st) != 0, 0)) {
        perror("fstat failed on jobs file!\n");
        close(jobsFileFd);
        return -1;
    }
    size_t dataLen = strlen(dataTobeWritten);
    if (__builtin_expect(ftruncate(jobsFileFd, dataLen) != 0, 0)) {
        perror("ftruncate failed on jobs file!\n");
        close(jobsFileFd);
        return -1;
    }
    char* data = mmap(NULL, dataLen, PROT_WRITE | PROT_READ, MAP_SHARED, jobsFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on jobs file!\n");
        close(jobsFileFd);
        return -1;
    }
    while(!isJobFree()) {
        sleep(1);
    }
    if (__builtin_expect(syncJob(true) != 0, 0)) {
        perror("failed to lock jobs operation!\n");
        memcpy(data, dataTobeWritten, dataLen);
        msync(data, dataLen, MS_SYNC);
        munmap(data, dataLen);
        return -1;
    }
    memcpy(data, dataTobeWritten, dataLen);
    msync(data, dataLen, MS_SYNC);
    munmap(data, dataLen);
    close(jobsFileFd);
    if (__builtin_expect(syncJob(false) != 0, 0)) {
        perror("failed to unlock jobs operation!\n");
        return -1;
    }
    return 0;
}   

static inline __attribute__((always_inline, hot)) int removeContentFromJobsFile() {
    int jobsFileFd = open("data/jobs", O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(jobsFileFd == -1, 0)) {
        perror("failed to open jobs file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(jobsFileFd, &st) != 0, 0)) {
        perror("fstat failed on jobs file!\n");
        close(jobsFileFd);
        return -1;
    }
    if(__builtin_expect(ftruncate(jobsFileFd, 0) != 0, 0)) {
        perror("ftruncate for deletion failed on jobs file!\n");
        return -1;
    }
    return 0;
}




__attribute__((hot)) int save_jobs_daemon(job*** jobs) {
    if (__builtin_expect(jobs == NULL, 0)) {
        perror("failed invalid memory on jobs!\n");
        return -1;
    }
    if (__builtin_expect(numberOfJobs == 0 || *jobs == NULL, 0)) {
        if(__builtin_expect(removeContentFromJobsFile() != 0, 0)) {
            return -1;
        }
        return 0;
    }
    job** _jobs = (*jobs);
    char* dataTobeWritten = NULL;
    _Bool isFirst = true;
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfJobsDaemon; ++i) {
        if (__builtin_expect((i & 31) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*jobs)[i + 32], 0, 3);
        }
        if (__builtin_expect(isFirst, 0)) {
            size_t size = snprintf(NULL, 0, "%s^%s#%li$%li@%i\n", _jobs[i]->name, _jobs[i]->runnableFile, _jobs[i]->lastTimeRunned, _jobs[i]->secondsInterval, _jobs[i]->pid);
            if (__builtin_expect(size <= 0, 0)) {
                perror("failed to calculate the size!\n");
                exit_program(1)
            }
            char* tmpData = malloc(size + 1);
            if (__builtin_expect(tmpData == NULL, 0)) {
                perror("failed to stringify job!\n");
                exit_program(1)
            }
            snprintf(tmpData, size + 1, "%s^%s#%li$%li@%i\n", _jobs[i]->name, _jobs[i]->runnableFile, _jobs[i]->lastTimeRunned, _jobs[i]->secondsInterval, _jobs[i]->pid);
            dataTobeWritten = tmpData;
            isFirst = false;
        } else {
            size_t size = snprintf(NULL, 0, "%s%s^%s#%li$%li@%i\n", dataTobeWritten, _jobs[i]->name, _jobs[i]->runnableFile, _jobs[i]->lastTimeRunned, _jobs[i]->secondsInterval, _jobs[i]->pid);
            if (__builtin_expect(size <= 0, 0)) {
                perror("failed to calculate the size!\n");
                exit_program(1)
            }
            char* tmpData = malloc(size + 1);
            if (__builtin_expect(tmpData == NULL, 0)) {
                perror("failed to stringify job!\n");
                exit_program(1)
            }
            snprintf(tmpData, size + 1, "%s%s^%s#%li$%li@%i\n", dataTobeWritten, _jobs[i]->name, _jobs[i]->runnableFile, _jobs[i]->lastTimeRunned, _jobs[i]->secondsInterval, _jobs[i]->pid);
            char* before = dataTobeWritten;
            dataTobeWritten = tmpData;
            free(before);
            before = NULL;
            tmpData = NULL;
        }
    }

    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );
    
    if (__builtin_expect(writeData(dataTobeWritten) != 0, 0)) {
        return -1;
    }
    free(dataTobeWritten);
    dataTobeWritten = NULL;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    return 0;
}