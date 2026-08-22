#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


__attribute__((hot)) __uint32_t getDaemonPid() {
    int fd = open("data/job_daemon_pid", O_CREAT | O_RDONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open job deamon pid file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on job daemon pid file!\n");
        return -1;
    }
    char pidBuff[40];
    ssize_t readBytes = read(fd, pidBuff, 39);
    pidBuff[readBytes] = '\0';
    close(fd);
    return atoi(pidBuff);
}