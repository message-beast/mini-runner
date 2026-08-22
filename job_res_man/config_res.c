#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "../res_man/utils/helper.h"
#include "../res_format/cpu_format.h"
#include "../res_format/mem_types.h"

DECLARE_128_T

static inline __attribute__((always_inline, hot)) int limitCpuOnly(__uint64_t cpuLimit) {
    int fd = open("data/job_cpu_limit", O_CREAT | O_WRONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job cpu limit file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on job cpu limit file!\n");
        close(fd);
        return -1;
    }
    size_t size = snprintf(NULL, 0, "%li", cpuLimit);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate size of cpu limit string!\n");
        return -1;
    }
    if (__builtin_expect(ftruncate(fd, size) != 0, 0)) {
        perror("ftruncate failed on job cpu limit file!\n");
        return -1;
    }
    char cpuLimitBuff[size + 1];
    snprintf(cpuLimitBuff, size + 1, "%li", cpuLimit);
    if (__builtin_expect(strlen(cpuLimitBuff) <= 0, 0)) {
        perror("failed to create cpu limit buffer!\n");
        return -1;
    }
    ssize_t written = write(fd, cpuLimitBuff, size);
    if (__builtin_expect(written <= 0 || written != size, 0)) {
        perror("failed to write cpu limit buffer to job cpu limit file!\n");
        return -1;
    }
    close(fd);
    return 0;
}


static inline __attribute__((always_inline, hot)) int limitMemOnly(__uint128_t memLimit) {
    int fd = open("data/job_mem_limit", O_CREAT | O_WRONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job memory limit file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on job memory limit file!\n");
        close(fd);
        return -1;
    }
    size_t size = snprintf(NULL, 0, "%s", format_128_t(memLimit));
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate size of memory limit string!\n");
        return -1;
    }
    if (__builtin_expect(ftruncate(fd, size) != 0, 0)) {
        perror("ftruncate failed on job memory limit file!\n");
        return -1;
    }
    char memLimitBuff[size + 1];
    snprintf(memLimitBuff, size + 1, "%s", format_128_t(memLimit));
    if (__builtin_expect(strlen(memLimitBuff) <= 0, 0)) {
        perror("failed to create memory limit buffer!\n");
        return -1;
    }
    ssize_t written = write(fd, memLimitBuff, size);
    if (__builtin_expect(written <= 0 || written != size, 0)) {
        perror("failed to write memory limit buffer to job memory limit file!\n");
        return -1;
    }
    close(fd);
    return 0;
}




__attribute__((hot)) int configJobRes(__uint128_t memLimit, __uint64_t cpuLimit, _Bool limitMemory, _Bool limitCpu) {
    if (limitCpu && !limitMemory) {
        if (__builtin_expect(limitCpuOnly(cpuLimit) != 0, 0)) {
            printf("try again!\n");
            return -1;
        }
        printf("\033[32m_________________\033[33mConfigured Job Resource Limit\033[32m_______________________\n");
        printf("\033[31mcpu core limit: \033[33m%.3f\n", convertToCores(cpuLimit));
        printf("\033[32m_______________________________________________________________________\033[0m\n");
        return 0;
    } else if (!limitCpu && limitMemory) {
        if (__builtin_expect(limitMemOnly(memLimit) != 0, 0)) {
            printf("try again!\n");
            return -1;
        }
        char* memLimitBuff = formatByte(memLimit);
        if (__builtin_expect(memLimitBuff == NULL, 0)) {
            perror("failed to allocate memory for memory limit buffer!\n");
            return -1;
        }
        printf("\033[32m_________________\033[33mConfigured Job Resource Limit\033[32m_______________________\n");
        printf("\033[31mmemory limit: \033[33m%s\n", memLimitBuff);
        printf("\033[32m_______________________________________________________________________\033[0m\n");
        free(memLimitBuff);
        return 0;
    }
    
    if (__builtin_expect(limitCpuOnly(cpuLimit) != 0, 0)) {
        printf("try again!\n");
        return -1;
    }
    if (__builtin_expect(limitMemOnly(memLimit) != 0, 0)) {
        printf("try again!\n");
        return -1;
    }
    char* memLimitBuff = formatByte(memLimit);
    if (__builtin_expect(memLimitBuff == NULL, 0)) {
        perror("failed to allocate memory for memory limit buffer!\n");
        return -1;
    }
    printf("\033[32m_________________\033[33mConfigured Job Resource Limit\033[32m_______________________\n");
    printf("\033[31mcpu core limit: \033[33m%.3f\n", convertToCores(cpuLimit));
    printf("\033[31mmemory limit: \033[33m%s\n", formatByte(memLimit));
    printf("\033[32m_______________________________________________________________________\033[0m\n");
    free(memLimitBuff);
    return 0;

}