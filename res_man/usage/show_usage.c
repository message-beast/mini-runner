#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../base/structure.h"
#include "../../base/config.h"
#include "parser/mem_parser.h"
#include <sys/mman.h>
#include "parser/mem_parser.h"
#include "parser/cpu_parser.h"



static inline __attribute__((always_inline, hot)) char* defineSpace(char* _s) {
    int len = 32 - strlen(_s);
    if (__builtin_expect(len <= 0, 0)) {
        return "\0";
    }
    char* string = malloc(len + 1);
    #pragma GCC ivdep
    for (register int i = 0; i < len; ++i) {
        string[i] = ' ';
    }
    string[len] = '\0';
    return string;

}



static inline __attribute__((always_inline, hot)) void freeMemInfo (meminfo* memInfo) {
    if (__builtin_expect(memInfo != NULL, 1)) {
        free(memInfo->disk);
        free(memInfo->heap);
        free(memInfo->stack);
        free(memInfo->vmrss);
        memInfo->disk = NULL;
        memInfo->heap = NULL;
        memInfo->stack = NULL;
        memInfo->vmrss = NULL;
        free(memInfo);
        memInfo = NULL;
    }
}


static inline __attribute__((always_inline, hot)) void freeResource (resource* res) {
    if (__builtin_expect(res != NULL, 1)) {
        freeMemInfo(res->mem);
        free(res);
        res = NULL;
    }
}

static inline __attribute__((always_inline, hot)) int showResourceUsage(char* serviceName, int pid) {
    size_t size = snprintf(NULL, 0, "/proc/%i/status", pid);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate process path string length!\n");
        return -1;
    }
    char statusPath[size + 1];
    snprintf(statusPath, size + 1, "/proc/%i/status", pid);
    if (__builtin_expect(strlen(statusPath) <= 0, 0)) {
        perror("failed to create a status path string!\n");
        return -1;
    }
    int fdStatusFile = open(statusPath, O_RDONLY);
    if (__builtin_expect(fdStatusFile == -1, 0)) {
        perror("failed to open status file!\n");
        return -1;
    }
    char data[1024];
    ssize_t readBytes = read(fdStatusFile, data, 1023);
    data[readBytes] = '\0';
    if (__builtin_expect(readBytes == 0, 0)) {
        perror("read failed!\n");
        return -1;
    }
    meminfo* memInfo = memoryParse(data);
    if (__builtin_expect(memInfo == NULL, 0)) {
        perror("failed to allocate space for memory info!\n");
        return -1;
    }
    size = snprintf(NULL, 0, "/proc/%i/stat", pid);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate stat path string length!\n");
        return -1;
    }
    char statPath[size + 1];
    snprintf(statPath, size + 1, "/proc/%i/stat", pid);
    if (__builtin_expect(strlen(statPath) <= 0, 0)) {
        perror("failed to create stat path string!\n");
        return -1;
    }
    int fdStat = open(statPath, O_RDONLY);
    if (__builtin_expect(fdStat == -1, 0)) {
        perror("failed to open stat file!\n");
        return -1;
    }
    char statData[512];
    readBytes = read(fdStat, statData, 511);
    statData[readBytes] = '\0';
    if (__builtin_expect(readBytes <= 0, 0)) {
        perror("failed to read stat file!\n");
        return -1;
    }
    int fdUptime = open("/proc/uptime", O_RDONLY);
    if (__builtin_expect(fdUptime == -1, 0)) {
        perror("failed to open uptime file!\n");
        return -1;
    }
    char uptimeData[50];
    readBytes = read(fdUptime, uptimeData, 49);
    uptimeData[readBytes] = '\0';
    if (__builtin_expect(readBytes <= 0, 0)) {
        perror("failed to read uptime Data!\n");
        return -1;
    }
    float cpuCores = numOfCores(statData, uptimeData);
    close(fdStatusFile);
    close(fdStat);
    close(fdUptime);
    resource* res = malloc(sizeof(resource));
    if (__builtin_expect(res == NULL, 0)) {
        perror("memory allocation for resource is failed!\n");
        freeMemInfo(memInfo);
        return -1;
    }
    res->mem = memInfo;
    res->numOfCores = cpuCores;

    char* s1 = defineSpace(res->mem->stack);
    char* s2 = defineSpace(res->mem->heap);
    char* s3 = defineSpace(res->mem->vmrss);
    char* s4 = defineSpace(res->mem->disk);
    printf("\033[32m||\033[33m-------------------------------------- \033[32mResource Usage for service %s \033[33m---------------------------------------\033[32m||\n", serviceName);
    printf("||                               \033[34mStack:                                         \033[33m%s\033[32m%s||\n", res->mem->stack, s1);
    printf("||                               \033[34mHeap:                                          \033[33m%s\033[32m%s||\n", res->mem->heap, s2);
    printf("||                               \033[34mRAM:                                           \033[33m%s\033[32m%s||\n", res->mem->vmrss, s3);
    printf("||                               \033[34mThreads:                                       \033[33m%i                               \033[32m||\n", res->mem->numThreads);
    printf("||                               \033[34mDisk:                                          \033[33m%s\033[32m%s||\n", res->mem->disk, s4);
    printf("||                               \033[34mCores:                                         \033[33m%.7f                       \033[32m||\n", res->numOfCores);
    printf("||\033[33m______________________________________________________________________________________________________________\033[32m||\033[0m\n");

    free(s1);
    free(s2);
    free(s3);
    free(s4);
    s1 = NULL;
    s2 = NULL;
    s3 = NULL;
    s4 = NULL;

    freeResource(res);
    return 0;
}




__attribute__((hot)) int showRsUsage(service*** __restrict__ services, char* __restrict__ serviceName) {
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            if (__builtin_expect(currentService->pid == 0, 0)) {
                fprintf(stderr, "\033[33m%s is not running!\033[0m\n", serviceName);
                return 0;
            }
            if (__builtin_expect(showResourceUsage(serviceName, currentService->pid) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
    fprintf(stderr, "fan not find any service with name %s\n", serviceName);
    return -1;
}