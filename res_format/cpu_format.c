#pragma optimize("O3")
#pragma optimize("fast-math")
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


__attribute__((hot)) __uint64_t convertToSeconds(char* cpuLimitBuff) {
    double cpuLimit = atof(cpuLimitBuff);
    #pragma STDC FP_CONTRACT ON
    return (__uint64_t)(cpuLimit * 100000);
}


__attribute__((hot)) double convertToCores(__uint64_t seconds) {
    #pragma STDC FP_CONTRACT ON
    return (double)seconds / 100000;
}