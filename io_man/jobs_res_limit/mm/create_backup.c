#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../../basic.h"
#include "../../base.h"
#include "../../../base/config.h"

#define _JOB_MEM_LIMIT_FILE __FILE_JOB_MEM_LIMIT
#define _JOB_MEM_LIMIT_COPY_FILE __FILE_JOB_MEM_LIMIT_COPY


int createJobRsMmLimitBackup() {
    if (__builtin_expect(doBackup(_JOB_MEM_LIMIT_FILE, _JOB_MEM_LIMIT_COPY_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}