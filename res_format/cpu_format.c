#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../arch/fma.h"

FMA(hot) __uint64_t convertToSeconds(char* cpuLimitBuff) {
    double cpuLimit = atof(cpuLimitBuff);
    #pragma STDC FP_CONTRACT ON
    return (__uint64_t)(cpuLimit * 100000);
}


FMA(hot) double convertToCores(__uint64_t seconds) {
    #pragma STDC FP_CONTRACT ON
    return (double)seconds / 100000;
}