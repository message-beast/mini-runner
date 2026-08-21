#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "../base/structure.h"
#include "../init/daemon_init.h"
#include "../base/config.h"
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#define check_write(_fd, _len, _wanted_length)\
    if (__builtin_expect(_len < _wanted_length || _len != _wanted_length, 0)) {\
        perror("write to pipe failed!");\
        close(_fd);\
        abort();\
    }\



#define is_divisible(x, y) y % x == 0



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





static inline __attribute__((always_inline, hot)) char* getPath(char* __restrict__ runnableFile, int size) {
    for (register int i = size - 1; i > 0; --i) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&runnableFile[i + 256], 0, 3);
        }
        if (__builtin_expect(runnableFile[i] == '/', 0)) {
            char* path = giveString(runnableFile, 0, i);
            if (__builtin_expect(path == NULL, 0)) {
                return NULL;
            }
            return path;
        }
    }
    return NULL;

}


static inline __attribute__((always_inline, hot)) void freeJobs(job*** jobs) {
    job** _jobs = (*jobs);
    if (__builtin_expect(_jobs != NULL, 1)) {
        for (register int i = 0; i < numberOfJobsDaemon; ++i) {
            job* currentJob = _jobs[i];
            free(currentJob->name);
            free(currentJob->runnableFile);
            free(currentJob);
            currentJob = NULL;
        }
        free(_jobs);
        _jobs = NULL;
    }
    numberOfJobsDaemon = 0;
    capacityOfJobsDaemon = 0;
} 



static inline __attribute__((always_inline, hot)) void* runJobsDaemon() {
    job** _jobs = malloc(sizeof(job*) * __INITIAL_SCALE_SIZE_OF_JOBS__);
    while(_jobs == NULL) {
        perror("failed to allocate memory for jobs retrying!\n");
        sleep(5);
        job** _jobs = malloc(sizeof(job*) * __INITIAL_SCALE_SIZE_OF_JOBS__);
    }
    job*** jobs = &_jobs;
    time_t wastedTime = 0;
    __uint64_t waitedTime = 0;
    while (1) {
        time_t now = time(NULL);
        if(__builtin_expect(loadJobsDaemon(jobs) != 0, 0)) {
            perror("\033[33mloading jobs failed!\033[0m\n");
            sleep(5);
            freeJobs(jobs);
            job** newJobMemory = malloc(sizeof(job*) * __INITIAL_SCALE_SIZE_OF_JOBS__);
            if (__builtin_expect(newJobMemory == NULL, 0)) {
                perror("memory allocation for jobs failed!\n");
                sleep(3);
                continue;
            }
            (*jobs) = newJobMemory;
            continue;
        }
        for (register int i = 0; i < numberOfJobsDaemon; ++i) {
            if (__builtin_expect((i & 31) == 0 || i == 0, 0)) {
                __builtin_prefetch(&(*jobs)[i + 128], 0, 3);
            }
            job* currentJob = (*jobs)[i];
            printf("waited time: %li, interval: %li\n", waitedTime, currentJob->secondsInterval);
            if (is_divisible(currentJob->secondsInterval, waitedTime)) {
                printf("found match!\n");
                int pipeFd[2];
                if (__builtin_expect(pipe(pipeFd) != 0, 0)) {
                    perror("pipe failed!\n");
                    continue;
                }
                char res[13];
                time_t t0 = time(NULL);
                pid_t pid = fork();
                if (pid == 0) {
                    close(pipeFd[0]);
                    char* path = getPath(currentJob->runnableFile, strlen(currentJob->runnableFile));
                    if (__builtin_expect(path == NULL, 0)) {
                        ssize_t written = write(pipeFd[1], "0\0", 2);
                        check_write(pipeFd[1], written, 2)
                        close(pipeFd[1]);
                        abort();
                    }
                    if (__builtin_expect(chdir(path) != 0, 0)) {
                        perror("failed to change directory!\n");
                        ssize_t written = write(pipeFd[1], "0\0", 2);
                        check_write(pipeFd[1], written, 2)
                        close(pipeFd[1]);
                        abort();
                    }
                    close(pipeFd[0]);
                    size_t len = snprintf(res, 13, "%i", getpid());
                    ssize_t written = write(pipeFd[1], res, len + 1);
                    check_write(pipeFd[1], written, len + 1)
                    close(pipeFd[1]);
                    close(STDERR_FILENO);
                    close(STDOUT_FILENO);
                    close(STDIN_FILENO);
                    execlp("bash", "bash", currentJob->runnableFile, NULL);
                    written = write(pipeFd[1], "0\0", 2);
                    check_write(pipeFd[1], written, 2)
                    close(pipeFd[1]);
                    abort();
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                    close(pipeFd[1]);
                    char resBuff[13];
                    ssize_t readBytes = read(pipeFd[0], resBuff, 13);
                    if (__builtin_expect(readBytes <= 0, 0)) {
                        perror("read on pipe failed!\n");
                        continue;
                    }
                    __uint32_t pid = atoi(resBuff);
                    currentJob->pid = pid;
                    currentJob->lastTimeRunned = time(NULL);
                }
                time_t t1 = time(NULL);
                wastedTime = t1 - t0;
            }
        }
        sleep(1);
        waitedTime++;
        freeJobs(jobs);
        job** newJobMemory = malloc(sizeof(job*) * __INITIAL_SCALE_SIZE_OF_JOBS__);
        if (__builtin_expect(newJobMemory == NULL, 0)) {
            perror("memory allocation for jobs failed!\n");
            return NULL;
        }
        (*jobs) = newJobMemory;
    }
    freeJobs(jobs);
    return NULL;
}





__attribute__((hot)) int launchDaemon() {
    pid_t pid = fork();
    pthread_t daemon;
    if (pid == 0) {
        if(__builtin_expect(pthread_create(&daemon, NULL, runJobsDaemon, NULL) != 0, 0)) {
            perror("failed to create daemon!\n");
            abort();
        }
        if(__builtin_expect(pthread_detach(daemon) != 0, 0)) {
            perror("failed to detach the daemon!\n");
            abort();
        }
    }
    if (pid > 0) {
        return 0;
    }
    setsid();
    close(STDERR_FILENO);
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    pthread_exit(NULL);
    return 0;
}