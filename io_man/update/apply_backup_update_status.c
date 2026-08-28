#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "../../basic.h"
#include "../base.h"

#define _UPDATE_STATUS_FILE "data/updateStatus"
#define _UPDATE_STATUS_COPY_FILE "data/updateStatus.backup"


int applyBackupUpdateStatus() {
    if (__builtin_expect(doBackup(_UPDATE_STATUS_COPY_FILE, _UPDATE_STATUS_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}