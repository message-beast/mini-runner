#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../base/structure.h"
#include "../../base/config.h"
#include "../../basic.h"
#include <string.h>
#include "../utils/cgrpv2/setup.h"
#include "../utils/cgrpv2/memory_limit.h"
#include "../utils/cgrpv1/setup.h"
#include "../utils/cgrpv1/memory_limit.h"
#include "../utils/helper.h"
#include <string.h>
#include "../level_utility/extreme.h"
#include "../level_utility/mid.h"
#include "../level_utility/low.h"
#include "../../arch/opt.h"
#include "../../utils_ops/create_limit.h"

DECLARE_128_T



#define SET_CPU_SIZE_WITH_MEM(name, pid, clim, blm, mem_size) _Generic(mem_size,\
                                                                       int: limitCpuAndMemory(name, pid, clim, blm, mem_size),\
                                                                       __uint64_t: limitCpuAndMemory_F_LRG(name, pid, clim, blm, mem_size),\
                                                                       __uint128_t: limitCpuAndMemory_F_EXTR(name, pid, clim, blm, mem_size))





OPT(hot) int setCpuResourceLimit_F_EXTR(service*** __restrict__ services, char* __restrict__ serviceName, float numberOfCpu, _Bool limitMemory, __uint128_t memBytes) {
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            printf("name: %s, pid: %i, numberofCpu: %.2f, limitMemory: %i, memBytes: %s\n", serviceName, (*services)[i]->pid, numberOfCpu, limitMemory, format_128_t(memBytes));
            __uint32_t pid = (*services)[i]->pid;
            if (__builtin_expect(pid == 0, 0)) {
                printf("\033[33mservice %s is not running\033[0m\n", serviceName);
                return 0;
            }
            if (__builtin_expect(SET_CPU_SIZE_WITH_MEM(serviceName, pid, numberOfCpu, limitMemory, memBytes) != 0, 0)) {
                return -1;
            }
            
            return 0;
        }
    }
    fprintf(stderr, "can not find any service with name %s\n", serviceName);
    return -1;
    
}





OPT(hot) int setCpuResourceLimit_F_LRG(service*** __restrict__ services, char* __restrict__ serviceName, float numberOfCpu, _Bool limitMemory, __uint64_t memBytes) {
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            printf("name: %s, pid: %i, numberofCpu: %.2f, limitMemory: %i, memBytes: %li\n", serviceName, (*services)[i]->pid, numberOfCpu, limitMemory, memBytes);
            __uint32_t pid = (*services)[i]->pid;
            if (__builtin_expect(pid == 0, 0)) {
                printf("\033[33mservice %s is not running\033[0m\n", serviceName);
                return 0;
            }
            if (__builtin_expect(SET_CPU_SIZE_WITH_MEM(serviceName, pid, numberOfCpu, limitMemory, memBytes) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
    fprintf(stderr, "can not find any service with name %s\n", serviceName);
    return -1;
    
}






OPT(hot) int setCpuResourceLimit(service*** __restrict__ services, char* __restrict__ serviceName, float numberOfCpu, _Bool limitMemory, int memBytesStr) {
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            printf("name: %s, pid: %i, numberofCpu: %.2f, limitMemory: %i, memBytes: %i\n", serviceName, (*services)[i]->pid, numberOfCpu, limitMemory, memBytesStr);
            __uint32_t pid = (*services)[i]->pid;
            if (__builtin_expect(pid == 0, 0)) {
                printf("\033[33mservice %s is not running\033[0m\n", serviceName);
                return 0;
            }
            if (__builtin_expect(SET_CPU_SIZE_WITH_MEM(serviceName, pid, numberOfCpu, limitMemory, memBytesStr) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
    fprintf(stderr, "can not find any service with name %s\n", serviceName);
    return -1;
    
}