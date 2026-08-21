#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#define true 1
#define false 0


__attribute__((hot)) int isJobFree() {
    int fd = open("data/jobs_sync", O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("filed to open jobs sync file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on jobs sync file!\n");
        return -1;
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