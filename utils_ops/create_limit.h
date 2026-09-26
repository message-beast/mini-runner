#include "../base/structure.h"
__attribute__((hot, aligned(64))) int createLimit(limit*** __restrict__ limits, char* __restrict__ name, __uint64_t cpuLimit, __uint128_t memLimit);