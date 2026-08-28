#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../arch/fma.h"

__uint64_t convertToSeconds(char* cpuLimitBuff) {
    printf("DEBUG %s: %i\n", __FILE__, __LINE__);
    double cpuLimit = atof(cpuLimitBuff);
    printf("DEBUG %s: %i\n", __FILE__, __LINE__);
    #pragma STDC FP_CONTRACT ON
    printf("DEBUG %s: %i\n", __FILE__, __LINE__);
    __uint64_t res = (__uint64_t)(cpuLimit * 100000);
    printf("DEBUG %s: %i\n", __FILE__, __LINE__);
    return res;
}


double convertToCores(__uint64_t seconds) {
    #pragma STDC FP_CONTRACT ON
    double res = (double)seconds / 100000;
    return res;
}