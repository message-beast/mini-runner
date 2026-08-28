#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../../basic.h"
#include "../../base.h"

#define _JOB_MEM_LIMIT_FILE "data/job_mem_limit"
#define _JOB_MEM_LIMIT_COPY_FILE "data/job_mem_limit.backup"


int applyJobRsMmLimitBackup() {
    if (__builtin_expect(doBackup(_JOB_MEM_LIMIT_COPY_FILE, _JOB_MEM_LIMIT_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}