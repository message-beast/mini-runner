#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/dir.h>
#include <errno.h>
#define DECLARE_FILE_COPY_F\
    static inline __attribute__((always_inline, hot)) int fileCopy(char* __restrict__ oldFile, char* __restrict__ newFile) {\
    int __fd_s = open(oldFile, O_RDWR);\
    if (__builtin_expect(__fd_s == -1, 0)) {\
        perror("failed to open the file to coped!");\
        fprintf(stderr, "file: %s\n", oldFile);\
        return -1;\
    }\
    int __fd_d = open(newFile, O_RDWR);\
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



__attribute__((hot)) int deleteDirectory(const char* path) {
    if (__builtin_expect(access(path, F_OK) != 0, 0)) {
        return 0;
    }
    int retry = 0;
    while (retry < 60) {
        if (__builtin_expect(rmdir(path) == 0, 1)) {
            return 0;
        }
        if (errno != EBUSY && errno != ENOTEMPTY) {
            perror("deleting directory failed with unexpected error!\n");
            return -1;
        }
        perror("failed to delete!\n");
        sleep(1);
        retry++;
    }
    
    return -1;
}