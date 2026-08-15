#include <stdio.h>
__attribute__((hot)) int limitMm(char* serviceName, __uint32_t servicePid, int memBytes);
__attribute__((hot)) int limitMm_F_LRG(char* serviceName, __uint32_t servicePid, __uint64_t memBytes);
__attribute__((hot)) int limitMm_F_EXTR(char* serviceName, __uint32_t servicePid, __uint128_t memBytes);