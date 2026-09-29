#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include "../arch/opt.h"
#include "../base/config.h"
#define true 1
#define false 0


OPT(hot) int isJobFree() {
    int fd = open(__FILE_JOBS_SYNC, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("filed to open jobs sync file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on jobs sync file!\n");
        return -1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        return true;
    }
    char* data = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on jobs sync file!\n");
        return -1;
    }
    if (__builtin_expect(strcmp(data, "free") == 0, 1)) {
        munmap(data, st.st_size);
        close(fd);
        return true;
    }
    munmap(data, st.st_size);
    close(fd);
    return false;
}