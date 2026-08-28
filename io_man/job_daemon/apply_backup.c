#include "../base.h"


#define _JOB_DAEMON_PID_FILE "data/job_daemon_pid"
#define _JOB_DAEMON_PID_COPY_FILE "data/job_daemon_pid.backup"

int applyJobDaemonPidBackup() {
    if (__builtin_expect(doBackup(_JOB_DAEMON_PID_COPY_FILE, _JOB_DAEMON_PID_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}