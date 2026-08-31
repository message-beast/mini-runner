#include <stdio.h>
#include "../io_man/base.h"
#include "config.h"


int createTimeShift() {
    _Bool done = doBackup(_JOB_CPU_LIMIT_PATH, _JOB_CPU_LIMIT_COPY_PATH) == 0 && doBackup(_JOB_CPU_LIMIT_BACKUP_PATH, _JOB_CPU_LIMIT_BACKUP_COPY_PATH) == 0 && doBackup(_JOB_DAEMON_PID_PATH, _JOB_DAEMON_PID_COPY_PATH) == 0 && doBackup(_JOB_DAEMON_PID_BACKUP_PATH, _JOB_DAEMON_PID_BACKUP_COPY_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_PATH, _JOB_MEM_LIMIT_COPY_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_BACKUP_PATH, _JOB_MEM_LIMIT_BACKUP_COPY_PATH) == 0 && doBackup(_JOB_PATH, _JOB_COPY_PATH) == 0 && doBackup(_JOB_BACKUP_PATH, _JOB_BACKUP_COPY_PATH) == 0 && doBackup(_JOB_SYNC_PATH, _JOB_SYNC_COPY_PATH) == 0 && doBackup(_JOB_SYNC_BACKUP_PATH, _JOB_SYNC_BACKUP_COPY_PATH) == 0 && doBackup(_PROJECT_PATH, _PROJECT_COPY_PATH) == 0 && doBackup(_PROJECT_BACKUP_PATH, _PROJECT_BACKUP_COPY_PATH) == 0 && doBackup(_UPDATE_STATUS_PATH, _UPDATE_STATUS_COPY_PATH) == 0 && doBackup(_UPDATE_STATUS_BACKUP_PATH, _UPDATE_STATUS_BACKUP_COPY_PATH) == 0;
    if (__builtin_expect(!done, 0)) {
        printf("failed to create time shift!\n");
        return -1;
    }
    printf("time shift created!\n");
    return 0;
}
