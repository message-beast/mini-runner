#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../base/config.h"
#include "../io_man/env/create_backup.h"
#include "../io_man/env/apply_backup.h"
#include <sys/signal.h>

#define ENV_FILE "data/env"
#define false 0
#define true 1

static void handleBackupForEnv() {
    if (__builtin_expect(applyEnvBackup() != 0, 0)) {
        perror("failed to create a backup!\n");
        return;
    }
}

static inline __attribute__((always_inline)) int writeData(char* __restrict__ content, int len) {
    int fd = open(ENV_FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open env file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failedon env file!\n");
        close(fd);
        return -1;
    }
    if (__builtin_expect(createEnvBackup() != 0, 0)) {
        perror("can not create backup for envs!\n");
        return -1;
    }
    struct sigaction sig;
    sig.sa_handler = handleBackupForEnv;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGTERM, &sig, NULL);
    sigaction(SIGSEGV, &sig, NULL);
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on env file!\n");
        close(fd);
        return -1;
    }
    char* data = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on env file!\n");
        close(fd);
        return -1;
    }
    memcpy(data, content, len);
    msync(data, len, MS_SYNC);
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );
    munmap(data, len);
    fsync(fd);
    close(fd);
    return 0;
}


static inline int __attribute__((always_inline)) eraseData() {
    int fd = open(ENV_FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open env file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failedon env file!\n");
        close(fd);
        return -1;
    }
    if (__builtin_expect(ftruncate(fd, 0) != 0, 0)) {
        perror("ftruncate failed on env file!\n");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}


int saveEnvs(env*** envs) {
    if (__builtin_expect(numberOfEnv == 0, 0)) {
        if (__builtin_expect(eraseData() != 0, 0)) return -1;
        return 0;
    }
    if (__builtin_expect(envs == NULL || *envs == NULL, 0)) {
        return -1;
    }
    char* dataToWrite = NULL;
    _Bool first = true;
    for (register int i = 0; i < numberOfEnv; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*envs)[i+64], 0, 3);
        }
        env* currentEnv = (*envs)[i];
        if (__builtin_expect(first, 0)) {
            size_t size = snprintf(NULL, 0, "%s`%s^%s\n", currentEnv->name, currentEnv->key, currentEnv->value);
            char* tmpData = malloc(size + 1);
            if (__builtin_expect(tmpData == NULL, 0)) {
                perror("failed toallocate memory for single data!\n");
                free(dataToWrite);
                return -1;
            }
            snprintf(tmpData, size + 1, "%s`%s^%s\n", currentEnv->name, currentEnv->key, currentEnv->value);
            dataToWrite = tmpData;
            first = false;
        } else {
            size_t size = snprintf(NULL, 0, "%s%s`%s^%s\n", dataToWrite, currentEnv->name, currentEnv->key, currentEnv->value);
            char* tmpData = malloc(size + 1);
            if (__builtin_expect(tmpData == NULL, 0)) {
                perror("failed to allocate memory for temp data!\n");
                free(dataToWrite);
                return -1;
            }
            char* beforeData = dataToWrite;
            snprintf(tmpData, size + 1, "%s%s`%s^%s\n", dataToWrite, currentEnv->name, currentEnv->key, currentEnv->value);
            dataToWrite = tmpData;
            free(beforeData);
        }
    }
    __asm__ volatile(
        "sfence"
        :
        :
        : "memory"
    );
    if (__builtin_expect(dataToWrite == NULL, 0)) {
        return -1;
    }
    if (__builtin_expect(writeData(dataToWrite, strlen(dataToWrite)) != 0, 0)) {
        free(dataToWrite);
        return -1;
    }
    free(dataToWrite);
    dataToWrite = NULL;
    return 0;
}