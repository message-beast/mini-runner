#include <stdio.h>
__attribute__((hot)) int limitCpuAndMemory(char* serviceName, __uint32_t servicePid, double cpuLimit, _Bool limitMemory, int memBytes);