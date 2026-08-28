#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../../basic.h"
#include "../../base.h"
#define _JOB_CPU_LIMIT_FILE "data/job_cpu_limit"
#define _JOB_CPU_LIMIT_COPY_FILE "data/job_cpu_limit.backup"


static inline __attribute__((always_inline, hot)) int applyJobLmBackup(char* cpuLimitBuff) {
    int fd = open(_JOB_CPU_LIMIT_FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed opnen job cpu lmit file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on cpu limit file!\n");
        exit_program(-1);
    }
    const int len = strlen(cpuLimitBuff);
    if (__builtin_expect(len <= 0, 0)) {
        perror("failed to calculate cpu lmit file string buffer length!\n");
        exit_program(-1)
    }
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on cpu limit file!\n");
        exit_program(-1)
    }
    char* data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on job cpu limit file!\n");
        exit_program(-1)
    }
    memcpy(data, cpuLimitBuff, len);
    msync(data, len, MS_SYNC);
    munmap(data, len);
    close(fd);
    return 0;
}


int applyJobRsLimitBackup() {
    if (__builtin_expect(doBackup(_JOB_CPU_LIMIT_COPY_FILE, _JOB_CPU_LIMIT_FILE) != 0, 0)) {
        return -1;
    }
    return 0;
}