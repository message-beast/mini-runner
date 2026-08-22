#include <stdio.h>
#include "stop_daemon.h"
#include "../job_ops/run_jobs.h"



__attribute__((hot)) int restartJobDaemon(job*** jobs) {
    if (__builtin_expect(stopJobDaemon(jobs) != 0, 0)) {
        perror("failed to stop running job daemon!\n");
        return -1;
    }
    if (__builtin_expect(launchDaemon() != 0, 0)) {
        perror("failed to start new job daemon!\n");
        return -1;
    }
}