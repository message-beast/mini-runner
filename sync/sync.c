#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

static inline __attribute__((always_inline, hot)) int lockJob(int fd) {
    char* content = "locked";
    int size = 6;
    if (__builtin_expect(ftruncate(fd, size) != 0, 0)) {
        perror("ftrunacte failed on job sync file!");
        return -1;
    }
    char* data = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on jobs sync file!\n");
        return -1;
    }
    memcpy(data, content, size);
    msync(data, size, MS_SYNC);
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    munmap(data, size);
    close(fd);
    return 0;
}



static inline __attribute__((always_inline, hot)) int unlockJob(int fd) {
    char* content = "free";
    int size = 4;
    if (__builtin_expect(ftruncate(fd, size) != 0, 0)) {
        perror("ftrunacte failed on job sync file!");
        return -1;
    }
    char* data = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on jobs sync file!\n");
        return -1;
    }
    memcpy(data, content, size);
    msync(data, size, MS_SYNC);
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    munmap(data, size);
    close(fd);
    return 0;
}




__attribute__((hot)) int syncJob(_Bool lock) {
    int fd = open("data/jobs_sync", O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job sync file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on jobs sync file!\n");
        return -1;
    }
    if (lock) {
        if (__builtin_expect(lockJob(fd) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(unlockJob(fd) != 0, 0)) {
            return -1;
        }
        return 0;
    }
    
}