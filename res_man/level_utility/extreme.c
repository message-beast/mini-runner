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
#include "../utils/cgrpv2/utils.h"
#include "../utils/cgrpv1/utils.h"

DECLARE_128_T
CREATE_PROCESS_REG_F
CREATE_PROCESS__F_V1

__attribute__((hot)) int limitCpuAndMemory_F_EXTR(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, __uint128_t memBytes) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        if(__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if(__builtin_expect(createProcess(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cpu.max", serviceName);
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
        int fd = open(cpuPath, O_WRONLY);
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
        int wanted_length = strlen(cpuBuff);
        ssize_t written = write(fd, cpuBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        if (!limitMemory) {
            return 0;
        }
        if (__builtin_expect(memoryLimit_F_EXTR(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if(__builtin_expect(createProcessV1(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cpu.cfs_quota_us", serviceName);
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
        int fd = open(cpuPath, O_WRONLY);
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
        int wanted_length = strlen(cpuLimitBuff);
        ssize_t written = write(fd, cpuLimitBuff, wanted_length);
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
        if (__builtin_expect(limitMemory_v1_F_EXTR(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}