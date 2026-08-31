#ifndef OPT_H
    #define OPT_H
#endif
#include "x86/features.h"
#ifdef __x86_64__
    #ifdef __AVX512__
    #define OPT(...)\
        __attribute__((target_clones("avx512,default"), __VA_ARGS__))
    #elifdef __AVX2__
    #define OPT(...)\
        __attribute__((target_clones("avx2,default"), __VA_ARGS__))
    #elifdef __AVX__
    #define OPT(...)\
        __attribute__((target_clones("avx,default"), __VA_ARGS__))
    #elifdef __SSE4_2
    #define OPT(...)\
        __attribute__((target_clones("sse4.2,default"), __VA_ARGS__))
    #elifdef __SSE4_1__
    #define OPT(...)\
        __attribute__((target_clones("sse4.1,default"), __VA_ARGS__))
    #elifdef __SSSE_3__
    #define OPT(...)\
        __attribute__((target_clones("ssse3,default"), __VA_ARGS__))
    #elifdef __SSE3__
    #define OPT(...)\
        __attribute__((target_clones("sse3,default"), __VA_ARGS__))
    #elifdef __SSE2__
    #define OPT(...)\
        __attribute__((target_clones("sse2,default"), __VA_ARGS__))
    #elifdef __SSE__
    #define OPT(...)\
        __attribute__((target_clones("sse,default"), __VA_ARGS__))
    #else
        #define OPT(...)\
            __attribute__((__VA_ARGS__))
    #endif
#endif