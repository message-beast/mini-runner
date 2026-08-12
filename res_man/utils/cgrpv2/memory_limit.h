//#pragma once
#pragma optimize("O3")
#include <stdio.h>
__attribute__((hot)) int memoryLimit_F_LRG(char* serviceName, __uint64_t memBytes);
__attribute__((hot)) int memoryLimit(char* serviceName, int memBytes);