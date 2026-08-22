#pragma optimize("O3")
#include <stdio.h>
#include "../base/structure.h"
#include "../job_res_man/get_limit.h"
#include <stdlib.h>
#include "../res_format/cpu_format.h"
#include "../res_format/mem_types.h"
#include "../res_man/utils/helper.h"
DECLARE_128_T
__attribute__((hot)) void showJobDaemonRsLimits() {
    job_rs_limit* limit = getLimit();
    if (__builtin_expect(limit == NULL, 0)) {
        perror("failed to allocate memory for limit!\n");
        return;
    }
    char* memoryLimit = formatByte(limit->memoryLimit);
    if (__builtin_expect(memoryLimit == NULL, 0)) {
        perror("failed to allocate memory for memory limit string!\n");
        return;
    }
    printf("\033[33m__________________\033[32mJob Runner Daemon Resource Limits\033[33m______________\n");
    printf("\033[33mcpu-core: \033[31m%.3f\n", convertToCores(limit->cpuLimit));
    printf("\033[33mmemory: \033[31m%s\n", memoryLimit);
    printf("\033[33m_________________________________________________________________\033[0m\n");
    free(limit);
    free(memoryLimit);
}