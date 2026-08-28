#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../basic.h"


static inline __attribute__((always_inline, hot)) int copyBackup(char* __restrict__ cpuLimitBuff, char* __restrict__ __dest) {
    int fd = open(__dest, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed opnen job cpu lmit backup file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on cpu limit backup file!\n");
        exit_program(-1);
    }
    const int len = strlen(cpuLimitBuff);
    if (__builtin_expect(len <= 0, 0)) {
        perror("failed to calculate cpu lmit file string buffer length!\n");
        exit_program(-1)
    }
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on cpu limit backup file!\n");
        exit_program(-1)
    }
    char* data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on job cpu limit backup file!\n");
        exit_program(-1)
    }
    memcpy(data, cpuLimitBuff, len);
    msync(data, len, MS_SYNC);
    munmap(data, len);
    close(fd);
    
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );

    return 0;
}


__attribute__((hot)) int doBackup(char* __restrict__ __src, char* __restrict__ __dest) {
    int fd = open(__src, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open cpu limit file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on cpu limit file!\n");
        exit_program(-1)
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on job cpu limit file!\n");
        exit_program(-1)
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        return 0;
    }
    if (__builtin_expect(copyBackup(data, __dest) != 0, 0)) {
        munmap(data, st.st_size);
        close(fd);
        return -1;
    }
    munmap(data, st.st_size);
    close(fd);
    return 0;
}