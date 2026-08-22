#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include "../daemon_utils/get_pid.h"
#include "../daemon_utils/save_pid.h"
#include "../base/structure.h"
#include "../base/config.h"
#include <signal.h>


static inline __attribute__((always_inline, hot)) int killJob(int pid) {
    if (__builtin_expect(kill(pid, 0) != 0, 1)) {
        return 0;
    }
    if (__builtin_expect(kill(pid, SIGKILL) != 0, 0)) {
        perror("failed to kill some process!\n");
        fprintf(stderr, "its pid %i\n", pid);
        return -1;
    }
    return 0;
}   

__attribute__((hot)) int stopJobDaemon(job*** jobs) {
    __uint32_t pid = getDaemonPid();
    if (__builtin_expect(pid == 0, 0)) {
        fprintf(stderr, "either job daemon is not running or it can not be parsed! try again!\n");
        return -1;
    }
    if (__builtin_expect(kill(pid, SIGKILL) != 0, 0)) {
        perror("failed to kill job daemon!\n");
        return -1;
    }
    if (__builtin_expect(setPid(0) != 0, 0)) {
        perror("failed to assign new pid of killed daemon!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfJobs; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*jobs)[i + 64], 0, 3);
        }
        job* currentJob = (*jobs)[i];
        if (__builtin_expect(currentJob->pid == 0, 0)) {
            continue;
        }
        killJob(currentJob->pid);
        currentJob->pid = 0;
    }
    return 0;
}