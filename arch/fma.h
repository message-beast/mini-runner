#ifdef __FMA__
    #define FMA(...)\
        __attribute__((target_clones("fma,default"), __VA_ARGS__))
#else
    #define FMA(...)\
        __attribute__((target_clones("default"), __VA_ARGS__))
#endif