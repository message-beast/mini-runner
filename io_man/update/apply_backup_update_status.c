#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "../../basic.h"
#include "../base.h"
#include "../../base/config.h"

#define _UPDATE_STATUS_FILE __FILE_UPDATE_STATUS
#define _UPDATE_STATUS_COPY_FILE __FILE_UPDATE_STATUS_COPY


int applyBackupUpdateStatus() {
    if (__builtin_expect(doBackup(_UPDATE_STATUS_COPY_FILE, _UPDATE_STATUS_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}