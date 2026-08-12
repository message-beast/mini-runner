#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../base/structure.h"
#include "../../base/config.h"
#include "../../basic.h"
#include <string.h>
#include "../utils/cgrpv2/setup.h"
#include "../utils/cgrpv2/memory_limit.h"
#include "../utils/cgrpv1/setup.h"
#include "../utils/cgrpv1/memory_limit.h"
#include "../utils/helper.h"
#include <string.h>


/*function for allocating above 4GB */
__attribute__((hot)) int limitCpuAndMemory_F_LRG(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, __uint64_t memBytes) {
    if (__builtin_expect(access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0, 0)) {
        if(__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("fauled to calculate the size of the string of new cgroup process!\n");
            return -1;
        }
        char newPrPath[size + 1];
        snprintf(newPrPath, sizeof(newPrPath), "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(invalidString(newPrPath), 0)) {
            perror("failed to create a string of new cgroup process path!\n");
            return -1;
        }
        int fd = open(newPrPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("can not open the system cgourp cgroup.procs!\n");
            return -1;
        }
        __asm__ volatile (
            "mfence"
            :
            :
            : "memory"
        );
        size = snprintf(NULL, 0, "%i", servicePid);
        if (__builtin_expect(size <= 0, 0)) {
            return -1;
        }
        char pidBuff[size + 1];
        snprintf(pidBuff, sizeof(pidBuff), "%i", servicePid);
        int wanted_length = strlen(pidBuff);
        ssize_t written = write(fd, pidBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("can not calculate the cpupath string size!\n");
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, sizeof(cpuPath), "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(invalidString(cpuPath), 0)) {
            perror("failed to create a cpupath string!\n");
            return -1;
        }
        fd = open(cpuPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("can not open the cpu path file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i 100000", (int)(cpuLimit * 100000));
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the pid length in string!\n");
            return -1;
        }

        char cpuBuff[size + 1];
        snprintf(cpuBuff, sizeof(cpuBuff), "%i 100000", (int)(cpuLimit * 100000));
        if (__builtin_expect(invalidString(cpuBuff), 0)) {
            perror("failed to create a cpu limit buff!\n");
            return -1;
        }
        wanted_length = strlen(cpuBuff);
        written = write(fd, cpuBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        if (!limitMemory) {
            return 0;
        }
        if (__builtin_expect(memoryLimit_F_LRG(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the service process path string length!\n");
            return -1;
        }
        char serPrPath[size + 1];
        snprintf(serPrPath, size + 1, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(invalidString(serPrPath), 0)) {
            perror("failed to create service process path string!\n");
            return -1;
        }
        int fd = open(serPrPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open service process file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i", servicePid);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the pid buffer string length!\n");
            return -1;
        }
        char pidBuff[size + 1];
        snprintf(pidBuff, size + 1, "%i", servicePid);
        if (__builtin_expect(invalidString(pidBuff), 0)) {
            perror("failed to create pid buffer string!\n");
            return -1;
        }
        int wanted_length = strlen(pidBuff);
        ssize_t written = write(fd, pidBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.cfs_quota_us", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu path string length!\n");
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, size + 1, "/sys/fs/cgroup/cpu/%s/cpu.cfs_quota_us", serviceName);
        if (__builtin_expect(invalidString(cpuPath), 0)) {
            perror("failed to create string for cpu path!\n");
            return -1;
        }
        fd = open(cpuPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open cpu file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i", (int)(cpuLimit * 100000));
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu limit buffer string length!\n");
            return -1;
        }
        char cpuLimitBuff[size + 1];
        snprintf(cpuLimitBuff, size + 1, "%i", (int)(cpuLimit * 100000));
        if (__builtin_expect(invalidString(cpuLimitBuff), 0)) {
            perror("failed to create cpu limit buffer string!\n");
            return -1;
        }
        wanted_length = strlen(cpuLimitBuff);
        written = write(fd, cpuLimitBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.cfs_period_us", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu limit path string length!\n");
            return -1;
        }
        char cpuLPath[size + 1];
        snprintf(cpuLPath, size + 1, "/sys/fs/cgroup/cpu/%s/cpu.cfs_period_us", serviceName);
        if (__builtin_expect(invalidString(cpuLPath), 0)) {
            perror("failed to create cpu limit path string!\n");
            return -1;
        }
        fd = open(cpuLPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open cpu limit file!\n");
            return -1;
        }
        written = write(fd, "100000", 6);
        CHECK_WRITE(fd, written, 6);
        close(fd);
        if (!limitMemory) {
            return 0;
        }
        if (__builtin_expect(limitMemory_v1_F_LRG(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}







/* function for allocating below 4.2 GB*/
__attribute__((hot)) int limitCpuAndMemory(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, int memBytes) {
    printf("runned!\n");
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        if(__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        printf("CATCH! %s: %i\n", __FILE__, __LINE__);
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("fauled to calculate the size of the string of new cgroup process!\n");
            return -1;
        }
        char newPrPath[size + 1];
        snprintf(newPrPath, sizeof(newPrPath), "/sys/fs/cgroup/%s/cgroup.procs", serviceName);
        if (__builtin_expect(invalidString(newPrPath), 0)) {
            perror("failed to create a string of new cgroup process path!\n");
            return -1;
        }
        int fd = open(newPrPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("can not open the system cgourp cgroup.procs!\n");
            return -1;
        }
        __asm__ volatile (
            "mfence"
            :
            :
            : "memory"
        );
        size = snprintf(NULL, 0, "%i", servicePid);
        if (__builtin_expect(size <= 0, 0)) {
            return -1;
        }
        char pidBuff[size + 1];
        snprintf(pidBuff, sizeof(pidBuff), "%i", servicePid);
        int wanted_length = strlen(pidBuff);
        ssize_t written = write(fd, pidBuff, wanted_length);
        CHECK_WRITE(fd, written , wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("can not calculate the cpupath string size!\n");
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, sizeof(cpuPath), "/sys/fs/cgroup/%s/cpu.max", serviceName);
        if (__builtin_expect(invalidString(cpuPath), 0)) {
            perror("failed to create a cpupath string!\n");
            return -1;
        }
        fd = open(cpuPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("can not open the cpu path file!\n");
            return -1;
        }
        printf("REACHED! %s: %i\n", __FILE__, __LINE__);
        size = snprintf(NULL, 0, "%i 100000", (int)(cpuLimit * 100000));
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the pid length in string!\n");
            return -1;
        }

        char cpuBuff[size + 1];
        snprintf(cpuBuff, sizeof(cpuBuff), "%i 100000", (int)(cpuLimit * 100000));
        if (__builtin_expect(invalidString(cpuBuff), 0)) {
            perror("failed to create a cpu limit buff!\n");
            return -1;
        }
        printf("buff to write to file: %s\n", cpuBuff);
        wanted_length = strlen(cpuBuff);
        written = write(fd, cpuBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        if (!limitMemory) {
            return 0;
        }
        if (__builtin_expect(memoryLimit(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the service process path string length!\n");
            return -1;
        }
        char serPrPath[size + 1];
        snprintf(serPrPath, size + 1, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);
        if (__builtin_expect(invalidString(serPrPath), 0)) {
            perror("failed to create service process path string!\n");
            return -1;
        }
        int fd = open(serPrPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open service process file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i", servicePid);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the pid buffer string length!\n");
            return -1;
        }
        char pidBuff[size + 1];
        snprintf(pidBuff, size + 1, "%i", servicePid);
        if (__builtin_expect(invalidString(pidBuff), 0)) {
            perror("failed to create pid buffer string!\n");
            return -1;
        }
        int wanted_length = strlen(pidBuff);
        ssize_t written = write(fd, pidBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.cfs_quota_us", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu path string length!\n");
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, size + 1, "/sys/fs/cgroup/cpu/%s/cpu.cfs_quota_us", serviceName);
        if (__builtin_expect(invalidString(cpuPath), 0)) {
            perror("failed to create string for cpu path!\n");
            return -1;
        }
        fd = open(cpuPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open cpu file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i", (int)(cpuLimit * 100000));
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu limit buffer string length!\n");
            return -1;
        }
        char cpuLimitBuff[size + 1];
        snprintf(cpuLimitBuff, size + 1, "%i", (int)(cpuLimit * 100000));
        if (__builtin_expect(invalidString(cpuLimitBuff), 0)) {
            perror("failed to create cpu limit buffer string!\n");
            return -1;
        }
        wanted_length = strlen(cpuLimitBuff);
        written = write(fd, cpuLimitBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.cfs_period_us", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu limit path string length!\n");
            return -1;
        }
        char cpuLPath[size + 1];
        snprintf(cpuLPath, size + 1, "/sys/fs/cgroup/cpu/%s/cpu.cfs_period_us", serviceName);
        if (__builtin_expect(invalidString(cpuLPath), 0)) {
            perror("failed to create cpu limit path string!\n");
            return -1;
        }
        fd = open(cpuLPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open cpu limit file!\n");
            return -1;
        }
        written = write(fd, "100000", 6);
        CHECK_WRITE(fd, written , 6);
        close(fd);
        if (!limitMemory) {
            return 0;
        }
        if (__builtin_expect(limitMemory_v1(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}



#define SET_CPU_SIZE_WITH_MEM(name, pid, clim, blm, mem_size) _Generic(mem_size,\
                                                                       int: limitCpuAndMemory(name, pid, clim, blm, mem_size),\
                                                                       __uint64_t: limitCpuAndMemory_F_LRG(name, pid, clim, blm, mem_size))



__attribute__((hot)) int setCpuResourceLimit(service*** __restrict__ services, char* __restrict__ serviceName, float numberOfCpu, _Bool limitMemory, char* __restrict__  memBytesStr) {
    
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            printf("name: %s, pid: %i, numberofCpu: %.2f, limitMemory: %i, memBytes: %i\n", serviceName, (*services)[i]->pid, numberOfCpu, limitMemory, atoi(memBytesStr));
            if (__builtin_expect(SET_CPU_SIZE_WITH_MEM(serviceName, (*services)[i]->pid, numberOfCpu, limitMemory, atoi(memBytesStr)) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
    return -1;
    
}