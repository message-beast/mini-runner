#define _POSIX_C_SOURCE 200809L
#include "../data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 400
#define MIN 40
#define true 1
#define false 0

#define SUPPORT(feature, cpu_features, first, content, chunk)\
    if (__builtin_expect(first, 0)) {\
        if (__builtin_cpu_supports(feature)) {\
            snprintf(cpu_features, 20, content);\
            first = false;\
            snprintf(chunk, MIN, "%s", cpu_features);\
        }\
    } else {\
        if(__builtin_cpu_supports(feature)) {\
            snprintf(cpu_features, MAX, "%s%s", chunk, content);\
            free(chunk);\
            size_t size = snprintf(NULL, 0, "%s", cpu_features);\
            chunk = malloc(size + 1);\
            snprintf(chunk, size + 1, "%s", cpu_features);\
        }\
    }\

void makeCpuSelection() {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-truncation"
    char cpu_features[MAX];
    char* chunk = malloc(MIN);
    _Bool first = true;
    SUPPORT("avx512f", cpu_features, first, "#define __AVX512__\n", chunk)
    SUPPORT("avx2", cpu_features, first, "#define __AVX2__\n", chunk)
    SUPPORT("avx", cpu_features, first, "#define __AVX__\n", chunk)
    SUPPORT("sse4.2", cpu_features, first, "#define __SSE4_2__\n", chunk)
    SUPPORT("sse4.1", cpu_features, first, "#define __SSE4_1__\n", chunk)
    SUPPORT("ssse3", cpu_features, first, "#define __SSSE_3__\n", chunk)
    SUPPORT("sse3", cpu_features, first, "#define __SSE3__\n", chunk)
    SUPPORT("fma", cpu_features, first, "#define __FMA__\n", chunk)
    free(chunk);
    #pragma GCC diagnostic pop
    int size = strlen(cpu_features);
    data = malloc(size + 1);
    if (__builtin_expect(data == NULL, 0)) {
        perror("memory allocation for data failed!\n");
        return;
    }
    memcpy(data, cpu_features, size);
    data[size] = '\0';
}