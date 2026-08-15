#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../basic.h"
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../file_sys_ops/file_system.h"
DECLARE_PROCESS_COPY_F

static inline __attribute__((always_inline)) int killProcess(pid_t pid) {
    if (__builtin_expect(kill(pid, 0) != 0, 0)) {
        printf("\033[32mprocess already killed by its id \033[33m%i\033[0m\n", pid);    
        return 0;
    }
    if (__builtin_expect(kill(pid, SIGKILL) != 0, 0)) {
        fprintf(stderr, "\033[31mFailed to kill the process with id of \033[33m%i\033[31mm!\033[0m\n", pid);
        return -1;
    }
    printf("\033[32mprocess killed by its id \033[33m%i\033[0m\n", pid);
    return 0;
}


static inline __attribute__((always_inline, hot)) int updateCgrp(char* serviceName) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the service cgroup path string length!\n");
            return -1;
        }
        char serCgrp[size + 1];
        snprintf(serCgrp, size + 1, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(strlen(serCgrp) <= 0, 0)) {
            perror("failed to create service cgroup path string!\n");
            return -1;
        }
        if (__builtin_expect(access(serCgrp, F_OK) != 0, 0)) {
            perror("cgroup not found!\n");
            return 0;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroup process file path string length!\n");
            return -1;
        }
        char serProFilePath[size + 1];
        snprintf(serProFilePath, size + 1, "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(strlen(serProFilePath) <= 0, 0)) {
            perror("failed to create service process file path string!\n");
            return -1;
        }
        if (__builtin_expect(processCopy_n(serProFilePath, 0) != 0, 0)) {
            perror("failed to put 0 in cgroup.procs!\n");
            return -1;
        }
        return 0;
    } else {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroup cpu service path string length!\n");
            return -1;
        }
        char cpuSerPath[size + 1];
        snprintf(cpuSerPath, size + 1, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(strlen(cpuSerPath) <= 0, 0)) {
            perror("failed to create a cpu cgroup service path string!\n");
            return -1;
        }
        if (__builtin_expect(access(cpuSerPath, F_OK) == 0, 1)) {
            size = snprintf(NULL, 0, "%s/cgroup.procs", cpuSerPath);
            if (__builtin_expect(size <= 0, 0)) {
                perror("failed to calculate cgroup cpu process path string lentgh!\n");
                return -1;
            }
            char cpuSerProPath[size + 1];
            snprintf(cpuSerProPath, size + 1, "%s/cgroup.procs", cpuSerPath);
            if (__builtin_expect(strlen(cpuSerProPath) <= 0, 0)) {
                perror("failed to create cpu service process file path string!\n");
                return -1;
            }
            if (__builtin_expect(processCopy_n(cpuSerProPath, 0) != 0, 0)) {
                return -1;
            }
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroup memory service path string length!\n");
            return -1;
        }
        char memorySerPath[size + 1];
        snprintf(memorySerPath, size + 1, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(strlen(memorySerPath) <= 0, 0)) {
            perror("failed to create a cpu cgroup service path string!\n");
            return -1;
        }
        if (__builtin_expect(access(memorySerPath, F_OK) == 0, 1)) {
            size = snprintf(NULL, 0, "%s/cgroup.procs", memorySerPath);
            if (__builtin_expect(size <= 0, 0)) {
                perror("failed to calculate cgroup cpu process path string lentgh!\n");
                return -1;
            }
            char memorySerProPath[size + 1];
            snprintf(memorySerProPath, size + 1, "%s/cgroup.procs", memorySerPath);
            if (__builtin_expect(strlen(memorySerProPath) <= 0, 0)) {
                perror("failed to create cpu service process file path string!\n");
                return -1;
            }
            if (__builtin_expect(processCopy_n(memorySerProPath, 0) != 0, 0)) {
                return -1;
            }
        }
        return 0;
    }
}




int stopService(service*** __restrict__ services, char* __restrict__ serviceName) {
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            DEBUG
            service* currentService = (*services)[i];
            printf("its pid is %i\n", currentService->pid);
            printf("service to stop:\n\t\033[31m|\033[33m-\033[34mname: \033[32m%s\n\t\033[31m|\033[33m-\033[34mgithubRepo: \033[32m%s\n\t\033[31m|\033[33m-\033[34mpid: \033[32m%i\n", currentService->name, currentService->githubRepo, currentService->pid);
            if (currentService->pid == 0) {
                return 0;
            }
            if (__builtin_expect(killProcess(currentService->pid) != 0, 0)) {
                perror("killing failed!\n");
                return -1;
            }
            if (__builtin_expect(updateCgrp(serviceName) != 0, 0)) {
                perror("cgourp failed!");
                return -1;
            }
            currentService->pid = 0;
            return 0;
        }
    }
    fprintf(stderr, "\033[31mcan not find any service with name \033[33m%s\033[0m\n", serviceName);
    return -1;
}