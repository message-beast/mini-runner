#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include "../res_man/utils/helper.h"
#include "../basic.h"
#include "../base/config.h"
#include "../io_man/limits/create_backup.h"
#include "../io_man/limits/apply_backup.h"
#include <signal.h>
#include "../arch/mem_barrier.h"
#include "../arch/arm_specs.h"

#define FILE __FILE_LIMITS

#define true 1
#define false 0

DECLARE_128_T


static void handleBackup(int signal) {
    if (__builtin_expect(applyBackupLimits() != 0, 0)) {
        perror("failed to apply snapshoots!\n");
    }
}


static inline __attribute__((always_inline, nonnull_if_nonzero(1, 2), access(read_only, 1, 2))) int writeData(char* content, int len) {
    int fd = open(FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open limits file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on limits file!\n");
        goto fail;
    }
    if (__builtin_expect(createBackupLimit() != 0, 0)) {
        perror("can not create snapshot before making a changes in limits!\n");
        return -1;
    }
    struct sigaction sig;
    sig.sa_handler = handleBackup;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sigaction(SIGINT, &sig, NULL);
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on limits file!\n");
        goto fail;
    }
    char* data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        goto fail;
    }
    memcpy(data, content, len);
    msync(data, len, MS_SYNC);
    munmap(data, len);
    fsync(fd);
    close(fd);
    return 0;

    fail:
        close(fd);
        return -1;
}


static inline __attribute__((always_inline)) int eraseData() {
    int fd = open(FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open limits file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on limits file!\n");
        goto fail;
    }
    if (__builtin_expect(createBackupLimit() != 0, 0)) {
        perror("can not create snapshot before making a changes in limits!\n");
        return -1;
    }
    struct sigaction sig;
    sig.sa_handler = handleBackup;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sigaction(SIGINT, &sig, NULL);
    if (__builtin_expect(ftruncate(fd, 0) != 0, 0)) {
        perror("ftruncate failed on limits file!\n");
        goto fail;
    }
    return 0;
    fail:
        close(fd);
        return -1;
}


__attribute__((hot)) int saveLimits(limit*** limits) {
    if (__builtin_expect((limits == NULL || *limits == NULL) && numberOfResLimits != 0, 0)) {
        return -1;
    }
    if (__builtin_expect(numberOfResLimits == 0, 0)) {
        if (__builtin_expect(eraseData() != 0, 0)) return -1;
        return 0;
    }
    char* dataToWrite = NULL;
    _Bool first = true;
    int dataSize = 0;
    for (register int i = 0; i < numberOfResLimits; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) __builtin_prefetch(&(*limits)[i+64], 0, 3);
        limit* currentLimit = (*limits)[i];
        if (__builtin_expect(first, 0)) {
            size_t size = snprintf(NULL, 0, "%s^%s#%li\n", currentLimit->name, format_128_t(currentLimit->memory), currentLimit->cpu);
            if (__builtin_expect(size <= 0, 0)) {
                perror("failed to calculate size of single data of limit string!\n");
                exit_program(-1)
            }
            char* tmp = malloc(size + 1);
            if (__builtin_expect(tmp == NULL, 0)) {
                perror("failed to allocate memory for single limit string!\n");
                exit_program(-1)
            }
            snprintf(tmp, size + 1, "%s^%s#%li\n", currentLimit->name, format_128_t(currentLimit->memory), currentLimit->cpu);
            dataToWrite = tmp;
            dataSize += size;
            first = false;
        } else {
            size_t size = snprintf(NULL, 0, "%s%s^%s#%li\n", dataToWrite, currentLimit->name, format_128_t(currentLimit->memory), currentLimit->cpu);
            if (__builtin_expect(size <= -1, 0)) {
                perror("failed to calculate size of single data of limit string!\n");
                exit_program(-1)
            }
            char* currentData = malloc(size + 1);
            if (__builtin_expect(currentData == NULL, 0)) {
                perror("failed to allocate memory for single limit string!\n");
                exit_program(-1)
            }
            snprintf(currentData, size + 1, "%s%s^%s#%li\n", dataToWrite, currentLimit->name, format_128_t(currentLimit->memory), currentLimit->cpu);
            char* beforeData = dataToWrite;
            dataToWrite = currentData;
            free(beforeData);
            dataSize += size;
        }
    }
    __MFENCE__
    __DSB_ISH__
    if (__builtin_expect(dataToWrite == NULL, 0)) return -1;

    if (__builtin_expect(writeData(dataToWrite, dataSize) != 0, 0)) {
        free(dataToWrite);
        return -1;
    }
    free(dataToWrite);
    return 0;
}