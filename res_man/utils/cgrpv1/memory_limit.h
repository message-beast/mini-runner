#pragma once
#include <stdio.h>
__attribute__((hot)) int limitMemory_v1_F_LRG(char* serviceName, __uint32_t servicePid, __uint64_t memBytes);

__attribute__((hot)) int limitMemory_v1(char* serviceName, __uint32_t servicePid, int memBytes);

__attribute__((hot)) int limitMemory_v1_F_EXTR(char* serviceName, __uint32_t servicePid, __uint128_t memBytes);