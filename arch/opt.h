#define OPT(...)\
    __attribute__((target_clones("avx512f,avx2,avx,sse4.2,sse4.1,ssse3,sse3,sse2,sse,default"), __VA_ARGS__))

