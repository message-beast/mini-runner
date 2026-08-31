#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../data.h"
#include <string.h>
#define _ARCH_CONFIG_FILE_PATH "arch/x86/features.h"

void configureFile() {
    int fd = open(_ARCH_CONFIG_FILE_PATH, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open arch config file file!\n");
        return;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on arch config file!\n");
        return;
    }
    const int len = strlen(data);
    if (__builtin_expect(len == 0, 0)) {
        printf("no modern cpu feature found!\n");
        return;
    }
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on arch config file!\n");
        return;
    }
    char* configData = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(configData == MAP_FAILED, 0)) {
        perror("mmap failed on arch config file!\n");
        return;
    }
    memcpy(configData, data, len);
    msync(data, len, MS_SYNC);
    munmap(configData, len);
    close(fd);
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
}