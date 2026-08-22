#pragma optimize("O3")
#pragma optimize("fast-math")
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
__attribute__((hot)) __uint128_t convertToBytes_JOB(char* memLimitBuff) {
    double memLimit = atof(memLimitBuff);
    int size = strlen(memLimitBuff);
    if (__builtin_expect(size <= 0, 0)) {
        perror("failed to calculate size!\n");
        return 0;
    }
    for (register int i = 0; i < size; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&memLimitBuff[i + 64], 0, 3);
        }
        if (__builtin_expect(memLimitBuff[i] == 'k', 0)) {
            #pragma STDC FP_CONTRACT ON
            return (__uint128_t) memLimit * 1024;
        } else if (__builtin_expect(memLimitBuff[i] == 'm', 0)) {
            #pragma STDC FP_CONTRACT ON
            return (__uint128_t) memLimit * 1024 * 1024;
        } else if (__builtin_expect(memLimitBuff[i] == 'g', 0)) {
            #pragma STDC FP_CONTRACT ON
            return (__uint128_t) memLimit * (__uint64_t) 1024 * 1024 * 1024;
        } else if (__builtin_expect(memLimitBuff[i] == 't', 0)) {
            #pragma STDC FP_CONTRACT ON
            return (__uint128_t) memLimit * (__uint64_t) 1024 * 1024 * 1024 * 1024;
        }
    }
    return memLimit;
}