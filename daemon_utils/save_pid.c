#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>


__attribute__((hot)) int setPid(__uint32_t pid) {
    int fd = open("data/job_daemon_pid", O_CREAT | O_WRONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job daemon pid file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on job daemon pid file!\n");
        close(fd);
        return -1;
    }
    size_t size = snprintf(NULL, 0, "%i", pid);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate the size of job daemon pid string!\n");
        close(fd);
        return -1;
    }
    if (__builtin_expect(ftruncate(fd, size) != 0, 0)) {
        perror("ftruncate failed on job daemon pid file!\n");
        return -1;
    }
    char pidBuff[size + 1];
    snprintf(pidBuff, size + 1, "%i", pid);
    if (__builtin_expect(strlen(pidBuff) <= 0, 0)) {
        perror("failed to create pid buff for job daemon!\n");
        close(fd);
        return -1;
    }
    ssize_t written = write(fd, pidBuff, size);
    if (__builtin_expect(written <= 0 || written != size, 0)) {
        perror("job daemon pid write failed!\n");
        while(written != size) {
            written = write(fd, pidBuff, size);
        }
    }
    close(fd);

    return 0;
}



