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

#define __ENV_COPY_FILE_PATH "data/env.backup"
#define __ENV_FILE_PATH "data/env"
#define true 1




__attribute__((hot)) int createEnvBackup() {
    if (__builtin_expect(numberOfEnv == 0, 0)) {
        return 0;
    }
    if (__builtin_expect(doBackup(__ENV_FILE_PATH, __ENV_COPY_FILE_PATH) != 0, 0)) {
        return -1;
    }
    return 0;
}

