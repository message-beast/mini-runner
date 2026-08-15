#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DECLARE_FILE_COPY_F\
    static inline __attribute__((always_inline, hot)) int fileCopy(char* __restrict__ oldFile, char* __restrict__ newFile) {\
    int __fd_s = open(oldFile, O_RDONLY);\
    if (__builtin_expect(__fd_s == -1, 0)) {\
        perror("failed to open the file to coped!");\
        fprintf(stderr, "file: %s\n", oldFile);\
        return -1;\
    }\
    int __fd_d = open(newFile, O_WRONLY);\
    if (__builtin_expect(__fd_d == -1, 0)) {\
        perror("failed to open destination file!\n");\
        fprintf(stderr, "file: %s\n", newFile);\
        return -1;\
    }\
    char buff [100];\
    ssize_t read_b = read(__fd_s, buff, 100);\
    buff[read_b] = '\0';\
    if (__builtin_expect(read_b <= 0, 0)) {\
        perror("failed to read source file!\n");\
        return -1;\
    }\
    ssize_t written = write(__fd_d, buff, read_b);\
    if (__builtin_expect(written <= 0 || written != read_b, 0)) {\
        perror("faile to write to destination file!\n");\
        return -1;\
    }\
    close(__fd_s);\
    close(__fd_d);\
    return 0;\
}\




#define DECLARE_COPY_PROCESS_F\
    static inline __attribute__((always_inline, hot)) int copyProcess(char* __restrict__ __old, char* __restrict __new) {\
        int from = open(__old, O_RDONLY);\
        if (__builtin_expect(from == -1, 0)) {\
            perror("failed to open source process file!\n");\
            return -1;\
        }\
        int to = open(__new, O_WRONLY);\
        struct timespec ts = {0, 1000000000};\
        while (to == -1) {\
            to = open(__new, O_WRONLY);\
            sleep(1);\
        }\
        if (__builtin_expect(to == -1, 0)) {\
            perror("failed to open destination process file!\n");\
            return -1;\
        }\
        char buff[50];\
        ssize_t n;\
        while ((n = read(from, buff, sizeof(buff) - 1)) > 0) {\
            buff[n] = '\0';\
            char* pid = strtok(buff, "\n");\
            while (pid) {\
                if (__builtin_expect(pid[0] == '\0', 0)) {\
                    pid = strtok(NULL, "\n");\
                }\
                printf("pid is: %s\n", pid);\
                int len = strlen(pid);\
                ssize_t written = write(to, pid, len);\
                while (written <= 0 || written != len) {\
                    written = write(to, pid, len);\
                    sleep(1);\
                }\
                if (__builtin_expect(written <= 0 || written != len, 0)) {\
                    perror("write failed!\n");\
                    return -1;\
                }\
                written = write(to, "\n", 1);\
                if (__builtin_expect(written <= 0 || written != 1, 0)) {\
                    perror("write failed!\n");\
                    return -1;\
                }\
                pid = strtok(NULL, "\n");\
            }\
        }\
        return 0;\
    }\



#define DECLARE_PROCESS_COPY_F\
    static inline __attribute__((always_inline, hot)) int processCopy_n(char* __restrict__ newFile, __uint32_t pid) {\
        int fd = open(newFile, O_WRONLY);\
        if (__builtin_expect(fd == -1, 0)) {\
            perror("failed to open new process file!\n");\
            return -1;\
        }\
        size_t size = snprintf(NULL, 0, "%i", pid);\
        if (__builtin_expect(size <= 0, 0)) {\
            perror("failed to calculate the pid string length!\n");\
            return -1;\
        }\
        char pidBuff[size + 1];\
        snprintf(pidBuff, size + 1, "%i", pid);\
        if (__builtin_expect(strlen(pidBuff) <= 0, 0)) {\
            perror("failed to create pid buffer string!\n");\
            return -1;\
        }\
        int len = strlen(pidBuff);\
        ssize_t written = write(fd, pidBuff, len);\
        if (__builtin_expect(written <= 0 || written != len, 0)) {\
            perror("failed to write to new process file!\n");\
            return -1;\
        }\
        return 0;\
    }\




__attribute__((hot)) int deleteDirectory(const char* path);