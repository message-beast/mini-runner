#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include "../../basic.h"
#define _UPDATE_STATUS_FILE "data/updateStatus"
#define _UPDATE_STATUS_FILE_COPY "data/updateStatus.backup"
#include "../base.h"



int createBackupForUpdateStatus() {
    if (__builtin_expect(doBackup(_UPDATE_STATUS_FILE, _UPDATE_STATUS_FILE_COPY) != 0, 0)) {
        return -1;
    }
    return 0;
}