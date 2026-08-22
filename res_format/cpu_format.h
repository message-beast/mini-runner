#pragma once
#pragma optimiez("O3")
#pragma optimize("fast-math")

__attribute__((hot)) __uint64_t convertToSeconds(char* cpuLimitBuff);

__attribute__((hot)) double convertToCores(__uint64_t seconds);