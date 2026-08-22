#pragma optimize("O3")
#include <stdio.h>
#include "../daemon_utils/get_pid.h"



__attribute__((hot)) void showDaemonStatus() {
    __uint32_t pid = getDaemonPid();
    if (__builtin_expect(pid == 0, 0)) {
        printf("\033[31mstoped\033[0m\n");
        return;
    }
    printf("\033[33mJOB STATUS\n");
    printf("\033[32mRunning\n");
    printf("\033[33mpid: \033[31m%i\033[0m\n", pid);
}