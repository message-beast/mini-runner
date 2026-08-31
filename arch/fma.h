#ifndef FMA_H
    #define FMA_H
#endif
#ifdef __x86_64__
    #ifdef __FMA__
        #define FMA(...)\
            __attribute__((target_clones("fma,default"), __VA_ARGS__))
    #else
        #define FMA(...)\
            __attribute__(( __VA_ARGS__))
    #endif
#endif