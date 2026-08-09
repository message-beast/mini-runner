#pragma optimize("03")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../base/structure.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "../base/config.h"
#include <sys/wait.h>
#include "../basic.h"
static inline __attribute__((always_inline, hot)) int writeUpdateAvialable(char* dataToWrite) {
    int updateStatusFileFd = open("data/updateStatus", O_CREAT | O_RDWR, 0644, NULL);
    if (__builtin_expect(updateStatusFileFd == -1, 0)) {
        perror("can not open update status file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(updateStatusFileFd, &st) != 0, 0)) {
        perror("fstat failed on update status file!\n");
        return -1;
    }
    int len = strlen(dataToWrite);
    if (__builtin_expect(len <= 0, 0)) {
        perror("can not get data to write length!\n");
        return -1;
    }
    if (__builtin_expect(ftruncate(updateStatusFileFd, len) != 0, 0)) {
        perror("ftruncate failed on updateStatus file!\n");
        return -1;
    }
    char* data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, updateStatusFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mem map failed on updateStatus file!\n");
        return -1;
    }
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    memcpy(data, dataToWrite, len);
    msync(data, len, MS_SYNC);
    munmap(data, len);
    close(updateStatusFileFd);
    return 0;
}


static inline __attribute__((always_inline, hot)) int update_service(service* foundService, _Bool stop) {
    if (__builtin_expect(foundService == NULL, 0)) {
        return -1;
    }
    if (stop) {
        if (__builtin_expect(foundService->pid != 0, 1)) {
            if (__builtin_expect(kill(foundService->pid, SIGINT) != 0, 0)) {
                fprintf(stderr, "\033[31mcan not stop a service\033[33m\"%s\"\033[0m\n", foundService->name);
            }
        }
    }
    int pipeFd[2];
    pipe(pipeFd);
    pid_t updatePid = fork();
    if (updatePid == 0) {
        close(pipeFd[0]);
        size_t size = snprintf(NULL, 0, "/var/lib/%s", foundService->name) + 1;
        if (__builtin_expect(size <= 0, 0)) {
            perror("command size failed to calculate!\n");
            write(pipeFd[1], "f", 2);
            close(pipeFd[1]);
            abort();
        }
        char path[size];
        snprintf(path, size, "/var/lib/%s", foundService->name);
        if (__builtin_expect(path == NULL, 0)) {
            perror("failed to generate path!\n");
            write(pipeFd[1], "f", 2);
            close(pipeFd[1]);
            abort();
        }
        if (__builtin_expect(chdir(path) != 0, 0)) {
            fprintf(stderr, "can not change the directory to %s\n", path);
            write(pipeFd[1], "f", 2);
            close(pipeFd[1]);
            abort();
        }
        if (__builtin_expect(system("sudo git pull") != 0, 0)) {
            write(pipeFd[1], "f", 2);
            fprintf(stderr, "\033[31mupdate failed!\033[0m\n");
        } else {
            write(pipeFd[1], "s", 2);
        }
        close(pipeFd[1]);
        abort();
    } else {
        wait(NULL);
        close(pipeFd[1]);
        char buff[2];
        read(pipeFd[0], buff, 2);
        if (__builtin_expect(strcmp(buff, "s") != 0, 0)) {
            return -1;
        }
    }
    return 0;
} 



void updateService(service*** __restrict__ services, char* __restrict__ serviceName, _Bool stop) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        return;
    }
    #pragma omp parallel for num_threads(8)
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            if (__builtin_expect(writeUpdateAvialable("updating") != 0, 0)) {
                exit_program(-1)
            }
            if (__builtin_expect(update_service((*services)[i], stop) != 0, 0)) {
                exit_program(-1)
            }
            if (__builtin_expect(writeUpdateAvialable("idle") != 0, 0)) {
                exit_program(-1)
            }
            exit_program(0);
        }
    }
}

__attribute__((hot)) void neglectUpdate() {
    if (__builtin_expect(writeUpdateAvialable("idle") != 0, 0)) {
        fprintf(stderr, "\033[31mfailed to neglect updates!\n");
        return;
    }
    return;
}