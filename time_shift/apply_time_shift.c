#include <stdio.h>
#include "config.h"
#include "../io_man/base.h"
#include "../base/structure.h"
#include "../init/main_init.h"
#include "../init/job_init.h"
#include "../base/config.h"
#include "../utils.h"
#include "../job_ops/free_jobs.h"
#include "clone_init.h"
#include <string.h>
#include "../utils_ops/free_clone_services.h"
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
        if (__builtin_expect(basic_v2_setup(newName) != 0, 0)) {
            return -1;
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





int applyTimeShift(service*** services, job*** jobs) {
    _Bool done = doBackup(_JOB_CPU_LIMIT_COPY_PATH, _JOB_CPU_LIMIT_PATH) == 0 && doBackup(_JOB_CPU_LIMIT_BACKUP_COPY_PATH, _JOB_CPU_LIMIT_BACKUP_PATH) == 0 && doBackup(_JOB_DAEMON_PID_COPY_PATH, _JOB_DAEMON_PID_PATH) == 0 && doBackup(_JOB_DAEMON_PID_BACKUP_COPY_PATH, _JOB_DAEMON_PID_BACKUP_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_COPY_PATH, _JOB_MEM_LIMIT_PATH) == 0 && doBackup(_JOB_MEM_LIMIT_BACKUP_COPY_PATH, _JOB_MEM_LIMIT_BACKUP_PATH) == 0 && doBackup(_JOB_COPY_PATH, _JOB_PATH) == 0 && doBackup(_JOB_BACKUP_COPY_PATH, _JOB_BACKUP_PATH) == 0 && doBackup(_JOB_SYNC_COPY_PATH, _JOB_SYNC_PATH) == 0 && doBackup(_JOB_SYNC_BACKUP_COPY_PATH, _JOB_SYNC_BACKUP_PATH) == 0 && doBackup(_PROJECT_COPY_PATH, _PROJECT_PATH) == 0 && doBackup(_PROJECT_BACKUP_COPY_PATH, _PROJECT_BACKUP_PATH) == 0 && doBackup(_UPDATE_STATUS_COPY_PATH, _UPDATE_STATUS_PATH) == 0 && doBackup(_UPDATE_STATUS_BACKUP_COPY_PATH, _UPDATE_STATUS_BACKUP_PATH) == 0 && doBackup(_ENV_COPY_PATH, _ENV_PATH) == 0 && doBackup(_ENV_BACKUP_COPY_PATH, _ENV_BACKUP_PATH) == 0;
    if (__builtin_expect(!done, 0)) {
        perror("failed to apply backup!\n");
        return -1;
    }
    service** servicesClone = malloc(sizeof(service*) * __INITIAL_SCALE_SIZE_OF_SERVICES__);
    if (__builtin_expect(servicesClone == NULL, 0)) {
        perror("failed to allocate memory for services clone!\n");
        return -1;
    }
    if (__builtin_expect(loadCloneServices(&servicesClone) != 0, 0)) {
        perror("failed to reload services!\n");
        freeCloneServices(&servicesClone);
        return -1;
    }
    for (register int i = 0; i < numberOfCloneProjects; ++i) {
        service* currentService = servicesClone[i];
        size_t size = snprintf(NULL, 0, "/var/lib/%s", currentService->name);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the size of the folder name of service!\n");
            freeCloneServices(&servicesClone);
            return -1;
        }
        char* path = malloc(size + 1);
        if (__builtin_expect(path == NULL, 0)) {
            perror("failed to allocate memory for the folder path name of the service!\n");
            freeCloneServices(&servicesClone);
            return -1;
        }
        snprintf(path, size + 1, "/var/lib/%s", currentService->name);
        for (register int j = 0; j < numberOfProjects; ++j) {
            if (__builtin_expect(strcmp(currentService->githubRepo, (*services)[j]->githubRepo) == 0, 0)) {
                if (__builtin_expect(strcmp((*services)[j]->name, currentService->name) == 0, 0)) {
                    continue;
                }
                char* oldName = (*services)[j]->name;
                size = snprintf(NULL, 0, "/var/lib/%s", oldName);
                if (__builtin_expect(size <= 0, 0)) {
                    perror("failed to calculate the size of the folder name of service!\n");
                    free(path);
                    freeCloneServices(&servicesClone);
                    exit_program(-1)
                }
                char* oldPath = malloc(size + 1);
                if (__builtin_expect(oldPath == NULL, 0)) {
                    perror("faled to allocate memory for oldPath!\n");
                    free(path);
                    freeCloneServices(&servicesClone);
                    exit_program(-1)
                }
                snprintf(oldPath, size + 1, "/var/lib/%s", oldName);
                if (__builtin_expect(rename(oldPath, path) != 0, 0)) {
                    perror("can not rename service folders while applying time shift. do it manually!\n");
                }
                if (__builtin_expect(renameCgrp(oldName, currentService->name, currentService->pid) != 0, 0)) {
                    perror("failed to rename cgroup naming changes!\n");
                }
                free(oldPath);
                break;
            }
        }
        free(path);
    }
    freeCloneServices(&servicesClone);
    
    printf("time shift successfully applied!\n");
    return 0;
}