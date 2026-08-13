#include <stdio.h>
__attribute__((hot)) int setMemoryLimit(service*** __restrict__ services, char* __restrict__ serviceName, int memBytes);
__attribute__((hot)) int setMemoryLimit_F_LRG(service*** __restrict__ services, char* __restrict__ serviceName, __uint64_t memBytes);
__attribute__((hot)) int setMemoryLimit_F_EXTR(service*** __restrict__ services, char* __restrict__ serviceName, __uint128_t memBytes);