#include <stdio.h>
#include "../../base/structure.h"
__attribute__((hot)) int setMemoryLimit(service*** __restrict__ services, limit*** __restrict__ limits, char* __restrict__ serviceName, int memBytes);
__attribute__((hot)) int setMemoryLimit_F_LRG(service*** __restrict__ services, limit*** __restrict__ limts, char* __restrict__ serviceName, __uint64_t memBytes);
__attribute__((hot)) int setMemoryLimit_F_EXTR(service*** __restrict__ services, limit*** __restrict__ limits, char* __restrict__ serviceName, __uint128_t memBytes);