#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../sync/sync.h"
#include "../../basic.h"
#include <string.h>
#include "../../base/config.h"
#include "../base.h"

#define __SERVICES_COPY_FILE_PATH "data/projects.backup"
#define __SERVICES_FILE_PATH "data/projects"
#define true 1




__attribute__((hot)) int createBackup() {
    if (__builtin_expect(numberOfProjects == 0, 0)) {
        return 0;
    }
    if (__builtin_expect(doBackup(__SERVICES_FILE_PATH, __SERVICES_COPY_FILE_PATH) != 0, 0)) {
        return -1;
    }
    return 0;
}

