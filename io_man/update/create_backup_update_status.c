#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include "../../basic.h"
#include "../base.h"
#include "../../base/config.h"

#define _UPDATE_STATUS_FILE __FILE_UPDATE_STATUS
#define _UPDATE_STATUS_FILE_COPY __FILE_UPDATE_STATUS_COPY



int createBackupForUpdateStatus() {
    if (__builtin_expect(doBackup(_UPDATE_STATUS_FILE, _UPDATE_STATUS_FILE_COPY) != 0, 0)) {
        return -1;
    }
    return 0;
}