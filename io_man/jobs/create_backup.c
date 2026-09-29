#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../basic.h"
#include <string.h>
#include "../../base/config.h"
#include "../base.h"

#define __JOBS_COPY_FILE_PATH __FILE_JOBS_COPY
#define __JOBS_FILE_PATH __FILE_JOBS
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

