#include "config.h"
#include "../io_man/base.h"


int applyTimeShift() {
    _Bool done = doBackup(_JOB_CPU_LIMIT_COPY_PATH, _JOB_CPU_LIMIT_PATH) == 0 && doBackup(_JOB_CPU_LIMIT_BACKUP_COPY_PATH, _JOB_CPU_LIMIT_BACKUP_PATH) == 0 && doBackup(_JOB_DAEMON_PID_COPY_PATH, _JOB_DAEMON_PID_PATH) == 0 && doBackup(_JOB_DAEMON_PID_BACKUP_COPY_PATH, _JOB_DAEMON_PID_BACKUP_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_COPY_PATH, _JOB_MEM_LIMIT_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_BACKUP_COPY_PATH, _JOB_MEM_LIMIT_BACKUP_PATH) == 0 && doBackup(_JOB_COPY_PATH, _JOB_PATH) == 0 && doBackup(_JOB_BACKUP_COPY_PATH, _JOB_BACKUP_PATH) == 0 && doBackup(_JOB_SYNC_COPY_PATH, _JOB_SYNC_PATH) == 0 && doBackup(_JOB_SYNC_BACKUP_COPY_PATH, _JOB_SYNC_BACKUP_PATH) == 0 && doBackup(_PROJECT_COPY_PATH, _PROJECT_PATH) == 0 && doBackup(_PROJECT_BACKUP_COPY_PATH, _PROJECT_BACKUP_PATH) == 0 && doBackup(_UPDATE_STATUS_COPY_PATH, _UPDATE_STATUS_PATH) == 0 && doBackup(_UPDATE_STATUS_BACKUP_COPY_PATH, _UPDATE_STATUS_BACKUP_PATH) == 0;
    if (__builtin_expect(!done, 0)) {
        return -1;
    }
    return 0;
}