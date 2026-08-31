#include <stdio.h>
#include "../../arch/arch.h"

#if ARCH == X86_64
    long syscall_close_x86_64(int fd);
    long syscall_fstatat_x86_64(int fd, const char* pathName, struct stat* statBuff, int flag);
    long syscall_getdents64_x86_64(int fd, void* dirp, size_t count);
    long syscall_openat_x86_64(int fd, const char* pathName, int flags, int mode);
    long syscall_rmdir_x86_64(const char* pathName);
    long syscall_unlinkat_x86_64(int fd, const char* pathName, int flags);
    #define syscall_close(fd) syscall_close_x86_64(fd)
    #define syscall_fstatat(fd, pathName, statBuff, flag) syscall_fstatat_x86_64(fd, pathName, statBuff, flag)
    #define syscall_getdents64(fd, dirp, count) syscall_getdents64_x86_64(fd, dirp, count)
    #define syscall_openat(fd, pathName, flags, mode) syscall_openat_x86_64(fd, pathName, flags, mode)
    #define syscall_rmdir(pathName) syscall_rmdir_x86_64(pathName)
    #define syscall_unlinkat(fd, pathName, flags) syscall_unlinkat_x86_64(fd, pathName, flags)
#elif ARCH == X86_32
    int syscall_close_x86_32(int fd);
    int syscall_fstatat_x86_32(int fd, const char* pathName, struct stat* statBuff, int flag);
    int syscall_getdents64_x86_32(int fd, void* dirp, size_t count);
    int syscall_openat_x86_32(int fd, const char* pathName, int flags, int mode);
    int syscall_rmdir_x86_32(const char* pathName);
    int syscall_unlinkat_x86_32(int fd, const char* pathName, int flags);
    #define syscall_close(fd) syscall_close_x86_32(fd)
    #define syscall_fstatat(fd, pathName. statBuff, flag) syscall_fstatat_x86_32(fd, pathName, statBuff, flag)
    #define syscall_getdents64(fd, dirp, count) syscall_getdents64_x86_32(fd, dirp, count)
    #define syscall_openat(fd, pathName, flags, mode) syscall_openat_x86_32(fd, pathName, flags, mode);
    #define syscall_rmdir(pathName) syscall_rmdir_x86_32(pathName)
    #define syscall_unlinkat(fd, pathName, flags) syscall_unlinkat_x86_32(fd, pathName, flags)
#endif