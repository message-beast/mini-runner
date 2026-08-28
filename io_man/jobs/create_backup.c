#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../basic.h"
#include <string.h>
#include "../../base/config.h"
#include "../base.h"

#define __JOBS_COPY_FILE_PATH "data/jobs.backup"
#define __JOBS_FILE_PATH "data/jobs"
#define true 1


__attribute__((hot)) int createBackupForJobs() {
    if (__builtin_expect(numberOfJobs == 0, 0)) {
        return 0;
    }
    if (__builtin_expect(numberOfJobs != 0, 1)) {
        if (__builtin_expect(doBackup(__JOBS_FILE_PATH, __JOBS_COPY_FILE_PATH) != 0, 0)) {
            return -1;
        }
    }
    return 0;
}

