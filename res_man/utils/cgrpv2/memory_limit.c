#pragma optimize("03")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../../base/structure.h"
#include "../../../base/config.h"
#include "../../../basic.h"
#include "../helper.h"
#include <string.h>

__attribute__((hot)) int memoryLimit_F_LRG(char* serviceName, __uint64_t memBytes) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the memPathSize!\n");
            return -1;
        }
        char memPath[size + 1];
        snprintf(memPath, sizeof(memPath), "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(invalidString(memPath), 0)) {
            perror("failed to create memPath string!\n");
            return -1;
        }
        int fd = open(memPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open the memoy cgroup file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%li", memBytes);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate memBuffSize!\n");
            return -1;
        }
        char memBuff[size + 1];
        snprintf(memBuff, sizeof(memBuff), "%li", memBytes);
        if (__builtin_expect(invalidString(memBuff), 0)) {
            perror("failed to create a string of memBuff!\n");
            return -1;
        }
        int wanted_length = strlen(memBuff);
        ssize_t written = write(fd, memBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        return 0;
}

__attribute__((hot)) int memoryLimit(char* serviceName, int memBytes) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the memPathSize!\n");
            return -1;
        }
        char memPath[size + 1];
        snprintf(memPath, sizeof(memPath), "/sys/fs/cgroup/%s/memory.max", serviceName);
        if (__builtin_expect(invalidString(memPath), 0)) {
            perror("failed to create memPath string!\n");
            return -1;
        }
        int fd = open(memPath, O_WRONLY);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open the memoy cgroup file!\n");
            return -1;
        }
        size = snprintf(NULL, 0, "%i", memBytes);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate memBuffSize!\n");
            return -1;
        }
        char memBuff[size + 1];
        snprintf(memBuff, sizeof(memBuff), "%i", memBytes);
        if (__builtin_expect(invalidString(memBuff), 0)) {
            perror("failed to create a string of memBuff!\n");
            return -1;
        }
        int wanted_length = strlen(memBuff);
        ssize_t written = write(fd, memBuff, wanted_length);
        CHECK_WRITE(fd, written, wanted_length);
        close(fd);
        return 0;
}