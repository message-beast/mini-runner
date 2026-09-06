#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../arch/fma.h"

__uint64_t convertToSeconds(char* cpuLimitBuff) {
    double cpuLimit = atof(cpuLimitBuff);
    #pragma STDC FP_CONTRACT ON
    __uint64_t res = (__uint64_t)(cpuLimit * 100000);
    return res;
}


double convertToCores(__uint64_t seconds) {
    #pragma STDC FP_CONTRACT ON
    double res = (double)seconds / 100000;
    return res;
}