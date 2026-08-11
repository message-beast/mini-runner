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
#include <string.h>
#include "../helper.h"
#define DIRECTORY 0755
__attribute__((hot)) int basic_v2_setup(char* serviceName) {
    if (__builtin_expect(access("/sys/fs/cgroup", F_OK) != 0, 0)) {
        if (__builtin_expect(mkdir("/sys/fs/cgroup", DIRECTORY) != 0, 0)) {
                perror("failed to create /sys/fs/cgroup directory!\n");
                return -1;
            }
    } else {
        printf("cgroup found!\n");
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate the folder paths string length!\n");
            return -1;
        }
        char path[size + 1];
        snprintf(path, sizeof(path), "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(invalidString(path), 0)) {
            perror("failed to allocate the stack with directory path string!\n");
            return -1;
        }
        if (__builtin_expect(access(path, F_OK) != 0, 0)) {
            if (__builtin_expect(mkdir(path, DIRECTORY) != 0, 0)) {
                perror("failed to create a cgroup/<your service> directory in /sys/fs");
                return -1;
            }
        }
        printf("finished creating!");
    }
    return 0;
}