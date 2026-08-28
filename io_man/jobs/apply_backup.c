#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../../sync/sync.h"
#include "../../basic.h"
#include <string.h>
#include "../../base/config.h"
#include "../base.h"

#define __JOBS_COPY_FILE_PATH "data/jobs.backup"
#define __JOBS_FILE_PATH "data/jobs"
#define __JOBS_SYNC_FILE_PATH "data/jobs_sync"


#define true 1
#define false 0


static inline __attribute__((always_inline, hot)) int writeToRealData(char* jobsBuff) {
    int fd = open(__JOBS_FILE_PATH, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open jobs file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on jobs file!\n");
        exit_program(-1)
    }
    const int len = strlen(jobsBuff);
    if (__builtin_expect(len <= 0, 0)) {
        perror("failed to calculate data length!\n");
        exit_program(-1)
    }
    if (__builtin_expect(ftruncate(fd, len) != 0, 0)) {
        perror("ftruncate failed on jobs file!\n");
        exit_program(-1)
    }

    char* data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on jobs file!\n");
        exit_program(-1)
    }

    if (__builtin_expect(syncJob(true) != 0, 0)) {
        perror("failed to lock jobs");
        exit_program(-1)
    }
    memcpy(data, jobsBuff, len);
    msync(data, len, MS_SYNC);
    munmap(data, len);
    close(fd);
    if (__builtin_expect(syncJob(false) != 0, 0)) {
        perror("failed to unlock jobs!\n");
        exit_program(-1)
    }
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );

    return 0;
}




int applyBackupForjobs() {
    if (__builtin_expect(numberOfJobs == 0, 0)) {
        return 0;
    }
    if (__builtin_expect(numberOfJobs != 0, 1)) {
        int fd = open(__JOBS_COPY_FILE_PATH, O_CREAT | O_RDWR, 0644);
        if (__builtin_expect(fd == -1, 0)) {
            perror("failed to open jobs backup file!\n");
            exit_program(-1)
        }        
        struct stat st;
        if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
            perror("fstat faile on job backup file!\n");
            exit_program(-1)
        }
        if (__builtin_expect(st.st_size == 0, 0)) {
            return 0;
        }
        char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (__builtin_expect(data == MAP_FAILED, 0)) {
            perror("mmpa failed on job backup file!\n");
            exit_program(-1)
        }
        if (__builtin_expect(writeToRealData(data) != 0, 0)) {
            munmap(data, st.st_size);
            close(fd);
            return -1;
        }
        munmap(data, st.st_size);
        close(fd);
    }
    return 0;
}

