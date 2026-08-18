#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../base/structure.h"
#include "../../base/config.h"
#include "../../res_format/mem_types.h"
#include "../../res_man/utils/helper.h"
#include "parser/cpu_parser.h"
#include "../../utils.h"
#include <string.h>
#include <math.h>
DECLARE_ROUND_2
DECLARE_CHARS_128_T
DECLARE_128_T



static inline __attribute__((always_inline, hot)) void printRsLimit(char* __restrict__ memSize, char* __restrict__ serviceName, float cpuLimit) {
    printf("\033[33m-------------- \033[32mResource Limit For \033[34m%s \033[33m---------------------\n", serviceName);
        printf("\033[32mMemory Limit: \033[33m%s\n", memSize);
        printf("\033[32mCpu Limit: \033[33m%.2f\n", cpuLimit);
        printf("-----------------------------------------------------------\033[0m\n");
}





static inline __attribute__((always_inline, hot)) char* giveString(char* string, int startIndex, int endingIndex) {
    int length = endingIndex - startIndex;
    char* newString = malloc(length + 1);
    if (__builtin_expect(newString == NULL, 0)) {
        perror("failed to allocate memory for the cutted string!\n");
        return NULL;
    }
    memcpy(newString, string + startIndex, length);
    newString[length] = '\0';
    return newString;
}



static inline __attribute__((always_inline, hot)) char* getMemorySize(char* data) {
    __uint128_t memBytes = format_128_char(data)
    char* memSize = formatByte(memBytes);
    if (__builtin_expect(memSize == NULL, 0)) {
        perror("failed to parse memory file!\n");
        return NULL;
    }
    return memSize;
}


static inline __attribute__((always_inline, hot)) float getCpuLimit(char* c1Buff, char* c2Buff) {
    int c1 = atoi(c1Buff);
    int c2 = atoi(c2Buff);
    if (__builtin_expect(c2 == 0, 0)) {
        return 0.0f;
    }
    return roundTo2(formatCpu(c1, c2));
}




static inline __attribute__((always_inline, hot)) int show(char* serviceName, int pid) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate service cgroupv2 path string length!\n");
            return -1;
        }
        char serCgrpPath[size + 1];
        snprintf(serCgrpPath, size + 1, "/sys/fs/cgroup/%s", serviceName);
        if (__builtin_expect(strlen(serCgrpPath) <= 0, 0)) {
            perror("failed to create service cgroup path string!\n");
            return -1;
        }
        if (__builtin_expect(access(serCgrpPath, F_OK) != 0, 0)) {
            perror("there is no resource limit on this service!\n");
            return 0;
        }
        size = snprintf(NULL, 0, "%s/memory.max", serCgrpPath);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate memory service path string length!\n");
            return -1;
        }
        char memPath[size + 1];
        snprintf(memPath, size + 1, "%s/memory.max", serCgrpPath);
        if (__builtin_expect(strlen(memPath) <= 0, 0)) {
            perror("failed to create memory path string!\n");
            return -1;
        }
        int memFd = open(memPath, O_RDONLY);
        if (__builtin_expect(memFd == -1, 0)) {
            perror("failed to open memory file!\n");
            return -1;
        }
        char buff[50];
        ssize_t readBytes = read(memFd, buff, 49);
        if (__builtin_expect(readBytes <= 0, 0)) {
            perror("read failed!\n");
            close(memFd);
            return -1;
        }
        buff[readBytes] = '\0';
        __uint128_t memBytes = format_128_char(buff)
        if (__builtin_expect(memBytes < 0, 0)) {
            perror("failed to calculate memoryBytes!\n");
            return -1;
        }
        char* memSize = formatByte(memBytes);
        if (__builtin_expect(memSize == NULL, 0)) {
            perror("failed to create memory size!\n");
            close(memFd);
            return -1;
        }
        size = snprintf(NULL, 0, "%s/cpu.max", serCgrpPath);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu path string length!\n");
            close(memFd);
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, size + 1, "%s/cpu.max", serCgrpPath);
        if (__builtin_expect(strlen(cpuPath) <= 0, 0)) {
            perror("failed to create cpu path string!\n");
            close(memFd);
            return -1;
        }
        int cpuFd = open(cpuPath, O_RDONLY);
        if (__builtin_expect(cpuFd == -1, 0)) {
            perror("failed to open cpu file!\n");
            close(memFd);
            return -1;
        }
        char cpuBuff[100];
        readBytes = read(cpuFd, cpuBuff, 99);
        if (__builtin_expect(readBytes <= 0, 0)) {
            perror("failed to read cpu file!\n");
            close(memFd);
            close(cpuFd);
            return -1;
        }
        cpuBuff[readBytes] = '\0';
        int c1 = 0;
        int c2 = 1;
        for (register int i = 0; i < readBytes; ++i) {
            if (__builtin_expect(cpuBuff[i] == ' ', 0)) {
                char* c1Buff = giveString(cpuBuff, 0, i);
                if (__builtin_expect(c1Buff == NULL, 0)) {
                    perror("failed to parse cpu file!\n");
                    close(memFd);
                    close(cpuFd);
                    return -1;
                }
                c1 = atoi(c1Buff);
                free(c1Buff);
                char* c2Buff = giveString(cpuBuff, i + 1, readBytes - 1);
                if (__builtin_expect(c2Buff == NULL, 0)) {
                    perror("failed to parse cpu file!\n");
                    close(memFd);
                    close(cpuFd);
                    return -1;
                }
                c2 = atoi(c2Buff);
                free(c2Buff);
            }
        }
        if (__builtin_expect(c1 == 0 && c2 == 1, 0)) {
            perror("failed to parse cpu file!\n");
            close(memFd);
            close(cpuFd);
            return -1;
        }
        float cpuLimit = roundTo2(formatCpu(c1, c2));
        printRsLimit(memSize, serviceName, cpuLimit);
        free(memSize);
        close(memFd);
        close(cpuFd);
        return 0;
    } else {
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate memory service cgroupv1 path string length!\n");
            return -1;
        }
        char serMemPath[size + 1];
        snprintf(serMemPath, size + 1, "/sys/fs/cgroup/memory/%s", serviceName);
        if (__builtin_expect(strlen(serMemPath) <= 0, 0)) {
            perror("failed to create service memory path string!\n");
            return -1;
        }
        int notFound = 0;
        int found = 0;
        if (__builtin_expect(access(serMemPath, F_OK) != 0, 0)) {
            printf("\033[33mfailed to get memry service path!\033[0m\n");
            notFound++;
            found = 1;
        }
        size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cpu service cgroup path string length!\n");
            return -1;
        }
        char serCpuPath[size + 1];
        snprintf(serCpuPath, size + 1, "/sys/fs/cgroup/cpu/%s", serviceName);
        if (__builtin_expect(strlen(serCpuPath) <= 0, 0)) {
            perror("failed to create service cpu path string!\n");
            return -1;
        }
        if (__builtin_expect(access(serCpuPath, F_OK) != 0, 0)) {
            printf("\033[33mwarning failed to get cpu service cgroup path!\n");
            notFound++;
            found = 2;
        }
        if (__builtin_expect(notFound > 1 && found == 0, 0)) {
            fprintf(stderr, "cgroup not found for this service!\n");
            return 0;
        }
        if (__builtin_expect(notFound == 1 && found > 0, 0)) {
                if (found == 1) {
                    size = snprintf(NULL, 0, "%s/memory.max", serMemPath);
                    if (__builtin_expect(size <= 0, 0)) {
                        perror("failed to calculate ser mem path string length!\n");
                        return -1;
                    }
                    char memPath[size + 1];
                    snprintf(memPath, size + 1, "%s/memory.max", serMemPath);
                    if (__builtin_expect(strlen(memPath) <= 0, 0)) {
                        perror("failed to create memory path string!\n");
                        return -1;
                    }
                    int memFd = open(memPath, O_RDONLY);
                    if (__builtin_expect(memFd == -1, 0)) {
                        perror("failed to open memory file!\n");
                        return -1;
                    }
                    char memBuff[51];
                    ssize_t readBytes = read(memFd, memBuff, 50);
                    if (__builtin_expect(readBytes <= 0, 0)) {
                        perror("failed to read memory file!\n");
                        close(memFd);
                        return -1;
                    }
                    memBuff[readBytes] = '\0';
                    char* memSize = getMemorySize(memBuff);
                    if (__builtin_expect(memSize == NULL, 0)) {
                        close(memFd);
                        return -1;
                    }
                    printRsLimit(memSize, serviceName, 0);
                    free(memSize);
                    close(memFd);
                    return 0;
                } else {
                    size = snprintf(NULL, 0, "%s/cpu.cfs_period_us", serCpuPath);
                    if (__builtin_expect(size <= 0, 0)) {
                        perror("failed to calculate cgroupv1 service cpu path string length!\n");
                        return -1;
                    }
                    char cpuPath[size + 1];
                    snprintf(cpuPath, size + 1, "%s/cpu.cfs_period_us", serCpuPath);
                    if (__builtin_expect(strlen(cpuPath) <= 0, 0)) {
                        perror("failed to create cgroupv1 cpu path string!\n");
                        return -1;
                    }
                    size = snprintf(NULL, 0, "%s/cpu.cfs_quota_us", serCpuPath);
                    if (__builtin_expect(size <= 0, 0)) {
                        perror("failed to calculate cgroupv1 service cpu quota path string length!\n");
                        return -1;
                    }
                    char cpuQuotaPath[size + 1];
                    snprintf(cpuQuotaPath, size + 1, "%s/cpu.cfs_period_us", serCpuPath);
                    if (__builtin_expect(strlen(cpuQuotaPath) <= 0, 0)) {
                        perror("failed to create cgroupv1 cpu quota path string!\n");
                        return -1;
                    }
                    int cpuFd = open(cpuPath, O_RDONLY);
                    if (__builtin_expect(cpuFd == -1, 0)) {
                        perror("failed to open cpu file!\n");
                        return -1;
                    }
                    char cpuBuff[51];
                    ssize_t readBytes = read(cpuFd, cpuBuff, 50);
                    if (__builtin_expect(readBytes <= 0, 0)) {
                        perror("failed to read cpu file!\n");
                        close(cpuFd);
                        return -1;
                    }
                    cpuBuff[50] = '\0';
                    int cpuQuotaFd = open(cpuQuotaPath, O_RDONLY);
                    if (__builtin_expect(cpuQuotaFd == -1, 0)) {
                        perror("failed to open cpu quota file!\n");
                        close(cpuFd);
                        return -1;
                    }
                    char cpuQuotaBuff[51];
                    readBytes = read(cpuQuotaFd, cpuQuotaBuff, 50);
                    if (__builtin_expect(readBytes <= 0, 0)) {
                        perror("failed to read cpu file!\n");
                        close(cpuFd);
                        close(cpuQuotaFd);
                        return -1;
                    }
                    cpuQuotaBuff[50] = '\0';
                    float cpuLimit = getCpuLimit(cpuBuff, cpuQuotaBuff);
                    printRsLimit("N/A", serviceName, cpuLimit);
                    close(cpuFd);
                    close(cpuQuotaFd);
                    return 0;
                }
        }


        size = snprintf(NULL, 0, "%s/memory.max", serMemPath);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate ser mem path string length!\n");
            return -1;
        }
        char memPath[size + 1];
        snprintf(memPath, size + 1, "%s/memory.max", serMemPath);
        if (__builtin_expect(strlen(memPath) <= 0, 0)) {
            perror("failed to create memory path string!\n");
            return -1;
        }
        int memFd = open(memPath, O_RDONLY);
        if (__builtin_expect(memFd == -1, 0)) {
            perror("failed to open memory file!\n");
            return -1;
        }
        char memBuff[51];
        ssize_t readBytes = read(memFd, memBuff, 50);
        if (__builtin_expect(readBytes <= 0, 0)) {
            perror("failed to read memory file!\n");
            close(memFd);
            return -1;
        }
        memBuff[readBytes] = '\0';
        char* memSize = getMemorySize(memBuff);
        if (__builtin_expect(memSize == NULL, 0)) {
            close(memFd);
            return -1;
        }


        size = snprintf(NULL, 0, "%s/cpu.cfs_period_us", serCpuPath);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroupv1 service cpu path string length!\n");
            free(memSize);
            close(memFd);
            return -1;
        }
        char cpuPath[size + 1];
        snprintf(cpuPath, size + 1, "%s/cpu.cfs_period_us", serCpuPath);
        if (__builtin_expect(strlen(cpuPath) <= 0, 0)) {
            perror("failed to create cgroupv1 cpu path string!\n");
            free(memSize);
            close(memFd);
            return -1;
        }
        size = snprintf(NULL, 0, "%s/cpu.cfs_quota_us", serCpuPath);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate cgroupv1 service cpu quota path string length!\n");
            free(memSize);
            close(memFd);
            return -1;
        }
        char cpuQuotaPath[size + 1];
        snprintf(cpuQuotaPath, size + 1, "%s/cpu.cfs_period_us", serCpuPath);
        if (__builtin_expect(strlen(cpuQuotaPath) <= 0, 0)) {
            perror("failed to create cgroupv1 cpu quota path string!\n");
            free(memSize);
            close(memFd);
            return -1;
        }
        int cpuFd = open(cpuPath, O_RDONLY);
        if (__builtin_expect(cpuFd == -1, 0)) {
            perror("failed to open cpu file!\n");
            free(memSize);
            close(memFd);
            return -1;
        }
        char cpuBuff[51];
        readBytes = read(cpuFd, cpuBuff, 50);
        if (__builtin_expect(readBytes <= 0, 0)) {
            perror("failed to read cpu file!\n");
            close(cpuFd);
            free(memSize);
            close(memFd);
            return -1;
        }
        cpuBuff[50] = '\0';
        int cpuQuotaFd = open(cpuQuotaPath, O_RDONLY);
        if (__builtin_expect(cpuQuotaFd == -1, 0)) {
            perror("failed to open cpu quota file!\n");
            close(cpuFd);
            free(memSize);
            close(memFd);
            return -1;
        }
        char cpuQuotaBuff[51];
        readBytes = read(cpuQuotaFd, cpuQuotaBuff, 50);
        if (__builtin_expect(readBytes <= 0, 0)) {
            perror("failed to read cpu file!\n");
            close(cpuFd);
            close(cpuQuotaFd);
            free(memSize);
            close(memFd);
            return -1;
        }
        cpuQuotaBuff[50] = '\0';
        float cpuLimit = getCpuLimit(cpuBuff, cpuQuotaBuff);
        printRsLimit(memSize, serviceName, cpuLimit);
        close(cpuFd);
        close(cpuQuotaFd);
        free(memSize);
        close(memFd);
        return 0;
    }
    return -1;
}






__attribute__((hot)) int showRsLimit(service*** __restrict__ services, char* __restrict__ serviceName) {
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            if (__builtin_expect(currentService->pid == 0, 0)) {
                fprintf(stderr, "\033[33mservice %s is not running!\n", serviceName);
                return 0;
            }
            if (__builtin_expect(show(serviceName, (*services)[i]->pid) != 0, 0)) {
                perror("showing resource limit failed!\n");
                return -1;
            }
            return 0;
        }
    }
    fprintf(stderr, "\033[33mcan not find any service with name \033[32m\"%s\"\033[0m\n", serviceName);
    return 0;
}