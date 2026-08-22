#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../base/structure.h"
#include <stdlib.h>
#include <sys/stat.h>

job_rs_limit* getLimit() {
    int fd = open("data/job_mem_limit", O_CREAT | O_RDONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job memory limit file!\n");
        return NULL;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on job memory limit file!\n");
        close(fd);
        return NULL;
    }
    char memLimit[130];
    ssize_t readBytes = read(fd, memLimit, 129);
    memLimit[readBytes] = '\0';
    fd = open("data/job_cpu_limit", O_CREAT | O_RDONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job cpu limit file!\n");
        return NULL;
    }
    struct stat st2;
    if (__builtin_expect(fstat(fd, &st2) != 0, 0)) {
        perror("fstat failed on job cpu limit file!\n");
        return NULL;
    }
    char cpuLimit[70];
    readBytes = read(fd, cpuLimit, 69);
    cpuLimit[readBytes] = '\0';
    __uint128_t memLimitNum = atoll(memLimit);
    __uint64_t cpuLimitNum = atol(cpuLimit);
    job_rs_limit* limit = malloc(sizeof(job_rs_limit));
    if (__builtin_expect(limit == NULL, 0)) {
        perror("memory allocation for job resource limit failed!\n");
        return NULL;
    }
    limit->cpuLimit = cpuLimitNum;
    limit->memoryLimit = memLimitNum;
    close(fd);
    return limit;
}