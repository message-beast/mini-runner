#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../file_sys_ops/file_system.h"
#include "../res_man/utils/cgrpv1/setup.h"
#include "../res_man/utils/cgrpv2/setup.h"
#include "../res_man/utils/cgrpv1/utils.h"
#include "../res_man/utils/cgrpv2/utils.h"
#include "../res_man/utils/helper.h"
DECLARE_FILE_COPY_F
DECLARE_COPY_PROCESS_F
DECLARE_PROCESS_COPY_F
CREATE_PROCESS__F_V1

static inline __attribute__((always_inline, hot)) int renameServiceProjectFoler(char* __restrict__ beforeName, char* __restrict__ newName) {
    size_t size = snprintf(NULL, 0, "/var/lib/%s", beforeName);
    size_t newSize = snprintf(NULL, 0, "/var/lib/%s", newName);
    if (__builtin_expect(size <= 0 || newSize <= 0, 0)) {
        perror("faile to calculate the size of the pathof old/new directory");
        return -1;
    }
    char beforeFolderName[size + 1];
    char newFolderName[newSize + 1];
    snprintf(beforeFolderName, size + 1, "/var/lib/%s", beforeName);
    snprintf(newFolderName, newSize + 1, "/var/lib/%s", newName);
    if (__builtin_expect(beforeFolderName == NULL || newFolderName == NULL, 0)) {
        perror("failed to get the string of full path of old/new project directory\n");
        return -1;
    }
    if (__builtin_expect(rename(beforeFolderName, newFolderName) != 0, 0)) {
        if (errno == ENOENT) {
            fprintf(stderr, "\033[31mcan not find service with folder of %s\n", beforeFolderName);
        } else if (errno == EACCES) {
            fprintf(stderr, "\033[31mpermission denied! \033[33m may be rerun with sudo\033[0m\n");
        } else if (errno == EXDEV) {
            fprintf(stderr, "\033[31mcross-device move is not supported!\n");
        } else {
            perror("unknown error occured!\n");
        }
        return -1;
    }
    return 0;

}


static inline __attribute__((always_inline, hot)) int renameCgrp(char* __restrict__ serviceName, char* __restrict__ newName, __uint32_t pid) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the cgroupv2 service path string length!\n");
            return -1;
        }
        char beforePath[size + 1];
        snprintf(beforePath, size + 1, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(strlen(beforePath) <= 0, 0)) {
            perror("failed to create a before servicename cgroupv2 path string!\n");
            return -1;
        }
        if (__builtin_expect(access(beforePath, F_OK) != 0, 0)) {
            perror("cgroup for this service not found!\n");
            return 0;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cgroup.procs", newName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the old process cgroup path string length!\n");
            return -1;
        }
        char newProPath[size + 1];
        snprintf(newProPath, size + 1, "/sys/fs/cgroup/%s/cgroup.procs", newName);
        if (__builtin_expect(strlen(newProPath) <= 0, 0)) {
            perror("failed to create before process path string!\n");
            return -1;
        }
        if (__builtin_expect(processCopy_n(newProPath, pid) != 0, 0)) {
            perror("failed to copy process file id!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the before cpu path string length!\n");
            return -1;
        }
        char beforeCpuFilePath[size + 1];
        snprintf(beforeCpuFilePath, size + 1, "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(strlen(beforeCpuFilePath) <= 0, 0)) {
            perror("failed to create a before cpu path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cpu.max", newName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the before cpu path string length!\n");
            return -1;
        }
        char newCpuFilePath[size + 1];
        snprintf(newCpuFilePath, size + 1, "/sys/fs/cgroup/%s/cpu.max", newName);
        if (__builtin_expect(strlen(newCpuFilePath) <= 0, 0)) {
            perror("failed to create a before cpu path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the before cpu path string length!\n");
            return -1;
        }
        char beforeMemoryFilePath[size + 1];
        snprintf(beforeMemoryFilePath, size + 1, "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(strlen(beforeMemoryFilePath) <= 0, 0)) {
            perror("failed to create a before cpu path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/memory.max", newName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the before cpu path string length!\n");
            return -1;
        }
        char newMemoryFilePath[size + 1];
        snprintf(newMemoryFilePath, size + 1, "/sys/fs/cgroup/%s/memory.max", newName);
        if (__builtin_expect(strlen(newMemoryFilePath) <= 0, 0)) {
            perror("failed to create a before cpu path string!\n");
            return -1;
        }
        printf("beforecpu: %s\nnewCpu: %s\n", beforeCpuFilePath, newCpuFilePath);
        if(__builtin_expect(fileCopy(beforeCpuFilePath, newCpuFilePath) != 0, 0)) {
            perror("copy failed!\n");
            return -1;
        }
        if (__builtin_expect(fileCopy(beforeMemoryFilePath, newMemoryFilePath) != 0, 0)) {
            perror("copy failed!\n");
            return -1;
        }
        sleep(1);
        if (__builtin_expect(deleteDirectory(beforePath) != 0, 0)) {
            perror("failed to remove the old cgroup!\n");
            return -1;
        }
        return 0;

    } else {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the string length of cpu cgroupv1 path");
            return -1;
        }
        char beforePath[size + 1];
        snprintf(beforePath, size + 1, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(strlen(beforePath) <= 0, 0)) {
            perror("failed to create a before cpu cgroup path string!\n");
            return -1;
        }
        if (__builtin_expect(access(beforePath, F_OK) != 0, 0)) {
            perror("no cgroup found!\n");
            return 0;
        }
        if (__builtin_expect(basic_v1_setup(newName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcessV1(newName, pid) != 0, 0)) {
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate string length for cgroup memory path!\n");
            return -1;
        }
        char beforeMemoryPath[size + 1];
        snprintf(beforeMemoryPath, size + 1, "/sys/fs/cgroup/memory/%s/memory.max", serviceName);
        if (__builtin_expect(strlen(beforeMemoryPath) <= 0, 0)) {
            perror("failed to create a before memory cgroup pathstring!\n");
            return -1;
        }
        if (__builtin_expect(access(beforeMemoryPath, F_OK) != 0, 0)) {
            perror("failed to get memory folder!\n");
            return 0;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", newName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate new path name for memory cgroup paths string length!\n");
            return -1;
        }
        char newMemoryPath[size + 1];
        snprintf(newMemoryPath, size + 1, "/sys/fs/cgroup/memory/%s", newName);
        if (__builtin_expect(strlen(newMemoryPath) <= 0, 0)) {
            perror("failed to create new memory path for cgroup string!\n");
            return -1;
        }
        if (__builtin_expect(fileCopy(beforeMemoryPath, newMemoryPath) != 0, 0)) {
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroupv1 cpu max file path string length!\n");
            return -1;
        }
        char beforeCpuPath[size + 1];
        snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.max", serviceName);
        if (__builtin_expect(strlen(beforeCpuPath) <= 0, 0)) {
            perror("failed to create before cgroupv1 pu max file path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.max", newName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroupv1 new cpu max file path string length!\n");
            return -1;
        }
        char newCpuPath[size + 1];
        snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.max", newName);
        if (__builtin_expect(strlen(newCpuPath) <= 0, 0)) {
            perror("failed to create before cgroupv1 new cpu max file path string!\n");
            return -1;
        }
        if (__builtin_expect(fileCopy(beforeCpuPath, newCpuPath) != 0, 0)) {
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu cgroupv1 path string length!\n");
            return -1;
        }
        char beforeCpuSerPath[size + 1];
        snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(strlen(beforeCpuSerPath) <= 0, 0)) {
            perror("failed to create before cpu service path string!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu cgroupv1 path string length!\n");
            return -1;
        }
        char beforeMemorySerPath[size + 1];
        snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(strlen(beforeMemorySerPath) <= 0, 0)) {
            perror("failed to create before cpu service path string!\n");
            return -1;
        }
        sleep(1);
        if (__builtin_expect(deleteDirectory(beforeMemorySerPath) != 0, 0)) {
            perror("delete failed on mem path cgrpv1!\n");
            return -1;
        }
        if (__builtin_expect(deleteDirectory(beforeCpuSerPath) != 0, 0)) {
            perror("delete failed on cpu path cgrpv1!\n");
            return -1;
        }
        return 0;
    }
    return 0;
}



void renameService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ newName) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        return;
    }
    if (__builtin_expect(serviceName == NULL || newName == NULL, 0)) {
        fprintf(stderr, "\033[31mservice name or new service name not provided!\033[0m\n");
        return;
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);   
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            char* beforeName = currentService->name;
            if (__builtin_expect(renameServiceProjectFoler(serviceName, newName) != 0, 0)) {
                perror("renaming service folder failed!\n");
                beforeName = NULL;
                return;
            }
            if (__builtin_expect(renameCgrp(serviceName, newName, currentService->pid) != 0, 0)) {
                perror("renaming cgroup failed!\n");
                beforeName = NULL;
                return;
            }
            currentService->name = strdup(newName);
            printf("\033[33mservice \033[31m\"%s\" \033[33msuccessfully renamed to \033[32m\"%s\"\033[0m\n", beforeName, currentService->name);
            free(beforeName);
            beforeName = NULL;
            return;
        }
    }
    fprintf(stderr, "can not find any service with name %s\n", serviceName);
    return;
}