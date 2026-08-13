#pragma optimize("O3")
#pragma optimize("fast-math")
#include <stdio.h>
#include "../../base/config.h"
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#define true 1
#define false 0
#define MAX_CPU_CORE 10


#define return_result(x)\
    if (__builtin_expect(x == true, 0)) {\
        return -1;\
    }\


#define max_core_violation(num, e_f)\
    if (__builtin_expect(num > MAX_CPU_CORE, 0)) {\
        e_f = __MAX_CORE_VIOLATION;\
    }

#define check_error(cpuBuff, endPtr, e_f)\
    if (__builtin_expect(errno == ERANGE, 0)) {\
        perror("the float goes beyond its limit!\n");\
        e_f = __ERROR_FORMATING_STR;\
        return -1;\
    } else if (__builtin_expect(endPtr == cpuBuff, 0)) {\
        perror("conversion failed!\n");\
        e_f = __ERROR_FORMATING_STR;\
        return -1;\
    } else if (__builtin_expect(strlen(endPtr) > 0, 0)) {\
        fprintf(stderr, "found garbage value! %s\n", endPtr);\
    }\


static inline __attribute__((always_inline, hot)) float roundTo2(float num) {
    return (roundf((num * 100)) / 100) ? ((roundf((num * 100)) / 100)) : 0.0;
}



__attribute__((hot)) float convertToFloat(char* cpuLimit, int* err_flg) {
    char* end;
    float cpuLimitInFloat = strtof(cpuLimit, &end);
    if (__builtin_expect(cpuLimitInFloat == 0.0, 0)) {
        fprintf(stderr, "you just give 0.0 on purpose or failed to convert!\n");
        *err_flg = __ERROR_FORMATING_STR;
    }
    check_error(cpuLimit, end, *err_flg)
    cpuLimitInFloat = roundTo2(cpuLimitInFloat);
    return cpuLimitInFloat;
}