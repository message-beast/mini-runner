#include <stdio.h>
__attribute__((hot)) int limitCpuAndMemory_F_EXTR(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, __uint128_t memBytes);