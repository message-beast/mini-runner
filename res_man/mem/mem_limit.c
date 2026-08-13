#pragma optimize("O3")
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
#include "./limiter.h"






#define SET_MEM_LIMIT(name, pid, m_size) _Generic(m_size,\
                                                  int: limitMm(name, pid, m_size),\
                                                  __uint64_t: limitMm_F_LRG(name, pid, m_size),\
                                                  __uint128_t: limitMm_F_EXTR(name, pid, m_size))








__attribute__((hot)) int setMemoryLimit(service*** __restrict__ services, char* __restrict__ serviceName, int memBytes) {
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            if (__builtin_expect(SET_MEM_LIMIT(serviceName, (*services)[i]->pid, memBytes) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
}


__attribute__((hot)) int setMemoryLimit_F_LRG(service*** __restrict__ services, char* __restrict__ serviceName, __uint64_t memBytes) {
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            if (__builtin_expect(SET_MEM_LIMIT(serviceName, (*services)[i]->pid, memBytes) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
}


__attribute__((hot)) int setMemoryLimit_F_EXTR(service*** __restrict__ services, char* __restrict__ serviceName, __uint128_t memBytes) {
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            if (__builtin_expect(SET_MEM_LIMIT(serviceName, (*services)[i]->pid, memBytes) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
}