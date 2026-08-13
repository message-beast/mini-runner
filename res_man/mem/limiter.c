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
#include "../level_utility/extreme.h"
#include "../level_utility/mid.h"
#include "../level_utility/low.h"
#include "../utils/cgrpv1/utils.h"
#include "../utils/cgrpv2/utils.h"

DECLARE_128_T
CREATE_PROCESS_REG_F
CREATE_PROCESS__F_V1


__attribute__((hot)) int limitMm(char* serviceName, __uint32_t servicePid, int memBytes) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        if (__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcess(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(memoryLimit(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcessV1(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(limitMemory_v1(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}

__attribute__((hot)) int limitMm_F_LRG(char* serviceName, __uint32_t servicePid, __uint64_t memBytes) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        if (__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcess(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(memoryLimit_F_LRG(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcessV1(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(limitMemory_v1_F_LRG(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}


__attribute__((hot)) int limitMm_F_EXTR(char* serviceName, __uint32_t servicePid, __uint64_t memBytes) {
    if (access("/sys/fs/cgroup/cgroup.controllers", F_OK) == 0) {
        if (__builtin_expect(basic_v2_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcess(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(memoryLimit_F_EXTR(serviceName, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    } else {
        if (__builtin_expect(basic_v1_setup(serviceName) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(createProcessV1(serviceName, servicePid) != 0, 0)) {
            return -1;
        }
        if (__builtin_expect(limitMemory_v1_F_EXTR(serviceName, servicePid, memBytes) != 0, 0)) {
            return -1;
        }
        return 0;
    }
}