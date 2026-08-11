#pragma once
#pragma optimize("03")
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

__attribute__((hot)) int limitCpuAndMemory_F_LRG(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, __uint64_t memBytes);
__attribute__((hot)) int limitCpuAndMemory(char* serviceName, __uint32_t servicePid, float cpuLimit, _Bool limitMemory, int memBytes);
#define SET_CPU_SIZE_WITH_MEM(name, pid, clim, blm, mem_size) _Generic(mem_size,\
                                                                       int limitCpuAndMemory(name, pid, clim, blm, mem_size),\
                                                                       __uint64_t limitCpuAndMemory_F_LRG(nam, pid, clim, blm, mem_size))

__attribute__((hot)) int setCpuResourceLimit(service*** __restrict__ services, char* __restrict__ serviceName, float numberOfCpu, _Bool limitMemory, char* __restrict__  memBytesStr);