#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "stop_service.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include "../utils.h"
#include "../file_sys_ops/file_system.h"
#include "../res_man/utils/cgrpv2/utils.h"
#include "../res_man/utils/cgrpv2/setup.h"
#include "../res_man/utils/helper.h"
CREATE_PROCESS_REG_F
DECLARE_PROCESS_COPY_F

[[nodiscard]] static inline __attribute__((always_inline, hot)) int killService(pid_t pid) {
    if (!pid) {
        return 0;
    }
    if (__builtin_expect(kill(pid, 0) != 0, 0)) {
        return 0;
    }
    if (__builtin_expect(kill(pid, SIGKILL) != 0, 0)) {
        fprintf(stderr, "can not kill service with its pid %i\n", pid);
        return -1;
    }
    return 0;
}




[[nodiscard]] static inline __attribute__((always_inline, hot)) int updateCgroup(char* __restrict__ serviceName, __uint32_t pid) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate service cgroup path string length!\n");
            return -1;
        }
        char serGrp[size + 1];
        snprintf(serGrp, size + 1, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(strlen(serGrp) <= 0, 0)) {
            perror("failed to create a service cgroup path string!\n");
            return -1;
        }
        if (__builtin_expect(access(serGrp, F_OK) != 0, 0)) {
            perror("cgroup not found!\n");
            return -1;
        }
        if (__builtin_expect(createProcess(serviceName, pid) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu cgrpv1 process file path string length!\n");
            return -1;
        }
        char cpuProPath[size + 1];
        snprintf(cpuProPath, size + 1, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(strlen(cpuProPath) <= 0, 0)) {
            perror("failed to create cpu proess path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu cgrpv1 process file path string length!\n");
            return -1;
        }
        char memoryProPath[size + 1];
        snprintf(cpuProPath, size + 1, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
        if (__builtin_expect(strlen(cpuProPath) <= 0, 0)) {
            perror("failed to create cpu proess path string!\n");
            return -1;
        }
        if (__builtin_expect(processCopy_n(cpuProPath, pid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(processCopy_n(memoryProPath, pid) != 0, 0)) {
            return -1;
        }
        return 0;
    }
    return -1;
}



__attribute__((hot)) int restartService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ bash, _Bool attach) {
    if (bash == NULL) {
        bash = "run.sh";
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            if (__builtin_expect(killService(currentService->pid) != 0, 0)) {
                perror("failed to stop!\n");
                return -1;
            }
            if (__builtin_expect(warmService(&((*services)[i]), bash, attach) != 0, 0)) {
                return -1;
            }
            __asm__ volatile (
                "sfence"
                :
                :
                : "memory"
            );
            if (__builtin_expect(updateCgroup(serviceName, currentService->pid) != 0, 0)) {
                if(__builtin_expect(killService(currentService->pid) != 0, 0)) {
                    perror("failed to kill the failed service!\n");
                    return -1;
                }
                return -1;
            }
            return 0;
        }
    }
    printf("\033[33mcan not find a service with name \"%s\"\n", serviceName);
    return -1;
}