#include <stdio.h>
#include <string.h>
#include <stdlib.h>

__attribute__((hot)) __uint64_t formatTime(char* timeBuff) {
    int size = strlen(timeBuff);
    __uint64_t formatedTimeSeconds = 0;
    for (register int i = 0; i < size; ++i) {
        if (__builtin_expect(timeBuff[i] == 'd', 0)) {
            double t = atof(timeBuff);
            formatedTimeSeconds = (__uint64_t) t * 24 * 3600;
        } else if (__builtin_expect(timeBuff[i] == 'h', 0)) {
            double t = atof(timeBuff);
            formatedTimeSeconds = (__uint64_t) t * 3600;
        } else if (__builtin_expect(timeBuff[i] == 'm', 0)) {
            double t = atof(timeBuff);
            formatedTimeSeconds = (__uint64_t) t * 60;
        }
    }
    return formatedTimeSeconds != 0 ? formatedTimeSeconds : atol(timeBuff);
}