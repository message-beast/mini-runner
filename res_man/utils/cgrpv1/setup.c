#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../helper.h"
#include <string.h>
#define DIRECTORY 0755
__attribute__((hot)) int basic_v1_setup(char* serviceName) {
    if (__builtin_expect(access("/sys/fs/cgroup", F_OK) != 0, 0)) {
        if (__builtin_expect(mkdir("/sys/fs/cgroup", DIRECTORY) != 0, 0)) {
            perror("can not create /sys/fs/cgroup directory!\n");
            return -1;
        }
    }
    if (__builtin_expect(access("/sys/fs/cgroup/cpu", F_OK) != 0, 0)) {
        if (__builtin_expect(mkdir("/sys/fs/cgroup/cpu", DIRECTORY) != 0, 0)) {
            perror("failed to create a direcory /sys/fs/cgroup/cpu!\n");
            return -1;
        }
    }
    if (__builtin_expect(access("/sys/fs/cgroup/memory", F_OK) != 0, 0)) {
        if (__builtin_expect(mkdir("/sysfs/cgroup/memory", DIRECTORY) != 0, 0)) {
            perror("failed to create a directory /sys/fs/cgroup/memory!\n");
            return -1;
        }
    }
    size_t cpuPathLen = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
    if (__builtin_expect(cpuPathLen <= 0, 0)) {
        perror("failed to calcultae the cpu path string length!\n");
        return -1;
    }
    char cpuPath[cpuPathLen + 1];
    snprintf(cpuPath, sizeof(cpuPath), "/sys/fs/cgroup/cpu/%s", serviceName);
    if (__builtin_expect(invalidString(cpuPath), 0)) {
        perror("failed to create the cpu path string!\n");
        return -1;
    }
    if (__builtin_expect(access(cpuPath, F_OK) != 0, 0)) {
        if (__builtin_expect(mkdir(cpuPath, DIRECTORY) != 0, 0)) {
            return -1;
        }
    }
    return 0;
}