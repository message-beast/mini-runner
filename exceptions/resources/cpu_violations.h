#include "../../base/config.h"
#define MAX_CPU_CORE 10
__attribute__((hot)) float convertToFloat(char* cpuLimit, int* err_flg);



#define max_core_violation(num, e_f)\
    if (__builtin_expect(num > MAX_CPU_CORE, 0)) {\
        e_f = __MAX_CORE_VIOLATION;\
    }\




    