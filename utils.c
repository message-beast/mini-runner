#define _POSIX_C_SOURCE 200809L
#include <sys/stat.h>
#include "utils_ops/load_project.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include "syscalls/calls/sys.h"

#define true 1
#define false 0


__attribute__((hot)) _Bool fileExists(const char* filePath) {
    struct stat st;
    return (stat(filePath, &st) == 0);
}


__attribute__((hot)) _Bool folderExists(const char* path) {
    struct stat st;
    if (__builtin_expect(stat(path, &st) != 0, 0)) {
        return false;
    }
    if (__builtin_expect(S_ISDIR(st.st_mode), 1)) {
        return true;
    }
    return false;
}



/*static inline __attribute__((always_inline)) long syscall_openat(int dir_fd, const char* pathName, int flags, int mode) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a" (ret)
        : "a" (SYS_openat), "D" (dir_fd), "S" (pathName), "d" (flags), "r10" (mode)
        : "rcx", "r11", "memory"
    );
    return ret;
}


static inline __attribute__((always_inlne)) long syscall_getdent64(int fd, void* dirp, size_t count) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a" (ret)
        : "a" (SYS_getdents64), "D" (fd), "S" (dirp), "d" (count)
        : "rcx", "r11", "memory"
    );
    return ret;
}

*/




static inline __attribute__((hot)) int deleteFolderFdRc(int fd) {
    char buff[8192];
    int bytes;


    while ((bytes = syscall_getdents64(fd, buff, sizeof(buff))) > 0) {
        struct dirent* entry;
        int offset = 0;
        while (offset < bytes) {
            entry = (struct dirent*)(offset + buff);
            if (__builtin_expect(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0, 0)) {
                offset += entry->d_reclen;
                continue;
            }

            struct stat st;
            if (__builtin_expect(syscall_fstatat(fd, entry->d_name, &st, AT_SYMLINK_NOFOLLOW) == 0, 1)) {
                if (S_ISDIR(st.st_mode)) {
                    int subFd = syscall_openat(fd, entry->d_name, O_DIRECTORY | O_RDONLY | O_NOFOLLOW, 0);
                    if (__builtin_expect(subFd != -1, 1)) {
                        deleteFolderFdRc(subFd);
                        syscall_close(subFd);
                        if (__builtin_expect(syscall_unlinkat(fd, entry->d_name, AT_REMOVEDIR) != 0, 0)) {
                            perror("failed to remove directory!\n");
                            return -1;
                        }
                    } else {
                        perror("failed to open sub directory!\n");
                        return -1;
                    }
                } else {
                    if (__builtin_expect(syscall_unlinkat(fd, entry->d_name, 0) != 0, 0)) {
                        perror("faliled to delete file!\n");
                        return -1;
                    }
                }
            } else {
                perror("fstat failed!\n");
                return -1;
            }
            offset += entry->d_reclen;

        }
    }
    if (__builtin_expect(bytes == -1, 0)) {
        perror("getdents64 failed!\n");
        return -1;
    }
    return 0;
}


__attribute__((hot)) int deleteFolderRecursievly(const char* folderPath) {
    int fd = open(folderPath, O_DIRECTORY | O_RDONLY | O_NOFOLLOW);
    if (__builtin_expect(fd == -1, 0)) {
        return -1;
    }
    int result = deleteFolderFdRc(fd);
    syscall_close(fd);
    if (__builtin_expect(syscall_rmdir(folderPath) != 0, 0)) {
        return -1;
    }
    return result;
}