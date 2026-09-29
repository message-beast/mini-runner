#include "../base.h"
#include "../../base/config.h"

#define _JOB_DAEMON_PID_FILE __FILE_JOB_DAEMON_PID
#define _JOB_DAEMON_PID_COPY_FILE __FILE_JOB_DAEMON_PID_COPY

int applyJobDaemonPidBackup() {
    if (__builtin_expect(doBackup(_JOB_DAEMON_PID_COPY_FILE, _JOB_DAEMON_PID_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}