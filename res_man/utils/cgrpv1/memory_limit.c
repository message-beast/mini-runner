#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "../helper.h"
#include <string.h>
__attribute__((hot)) int limitMemory_v1_F_LRG(char* serviceName, __uint32_t servicePid, __uint64_t memBytes) {
    size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate memory process id path string length!\n");
        return -1;
    }
    char memPrPath[size + 1];
    snprintf(memPrPath, size + 1, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
    if (__builtin_expect(invalidString(memPrPath), 0)) {
        perror("failed to create a memory process id path string!\n");
        return -1;
    }
    int fd = open(memPrPath, O_WRONLY);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open memory proces id file!\n");
        return -1;
    }
    size = snprintf(NULL, 0, "%i", servicePid);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to caculate the pid buffer string length!\n");
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
    size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s/memory.limit_in_bytes", serviceName);
    if (__builtin_expect(size <= 0, 0)) {
        perror("can not calculate memory path buffer stringlength!\n");
        return -1;
    }
    char memPath[size + 1];
    snprintf(memPath, size + 1, "/sys/fs/cgroup/memory/%s/memory.limit_in_bytes", serviceName);
    if (__builtin_expect(invalidString(memPath), 0)) {
        perror("failed to create memory path string!\n");
        return -1;
    }
    fd = open(memPath, O_WRONLY);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open memory path file!\n");
        return -1;
    }
    size = snprintf(NULL, 0, "%li", memBytes);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate the memory buffer string length!\n");
        return -1;
    }
    char memBuff[size + 1];
    snprintf(memBuff, size + 1, "%li", memBytes);
    if (__builtin_expect(invalidString(memBuff), 0)) {
        perror("failed to create memory buffer string!\n");
        return -1;
    }
    wanted_length = strlen(memBuff);
    written = write(fd, memBuff, wanted_length);
    CHECK_WRITE(fd, written, wanted_length);
    close(fd);
    return 0;
}



__attribute__((hot)) int limitMemory_v1(char* serviceName, __uint32_t servicePid, int memBytes) {
    size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate memory process id path string length!\n");
        return -1;
    }
    char memPrPath[size + 1];
    snprintf(memPrPath, size + 1, "/sys/fs/cgroup/memory/%s/cgroup.procs", serviceName);
    if (__builtin_expect(invalidString(memPrPath), 0)) {
        perror("failed to create a memory process id path string!\n");
        return -1;
    }
    int fd = open(memPrPath, O_WRONLY);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open memory proces id file!\n");
        return -1;
    }
    size = snprintf(NULL, 0, "%i", servicePid);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to caculate the pid buffer string length!\n");
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
    size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s/memory.limit_in_bytes", serviceName);
    if (__builtin_expect(size <= 0, 0)) {
        perror("can not calculate memory path buffer stringlength!\n");
        return -1;
    }
    char memPath[size + 1];
    snprintf(memPath, size + 1, "/sys/fs/cgroup/memory/%s/memory.limit_in_bytes", serviceName);
    if (__builtin_expect(invalidString(memPath), 0)) {
        perror("failed to create memory path string!\n");
        return -1;
    }
    fd = open(memPath, O_WRONLY);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open memory path file!\n");
        return -1;
    }
    size = snprintf(NULL, 0, "%i", memBytes);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate the memory buffer string length!\n");
        return -1;
    }
    char memBuff[size + 1];
    snprintf(memBuff, size + 1, "%i", memBytes);
    if (__builtin_expect(invalidString(memBuff), 0)) {
        perror("failed to create memory buffer string!\n");
        return -1;
    }
    wanted_length = strlen(memBuff);
    written = write(fd, memBuff, wanted_length);
    CHECK_WRITE(fd, written, wanted_length);
    close(fd);
    return 0;
}