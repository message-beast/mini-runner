#include "arch.h"

#if ARCH == X86_64 || ARCH == X86_32
    #define __MFENCE__ __asm__ volatile ("mfence" ::: "memory");
    #define __LFENCE__ __asm__ volatile ("lfence" ::: "memory");
    #define __SFENCE__ __asm__ volatile ("sfence" ::: "memory");
#elif ARCH == ARM_64 || ARCH == ARM_32
    #define __MFENCE__ __asm__ volatile ("DMB ISH" ::: "memory");
    #define __LFENCE__ __asm__ volatile ("DMB ISHLD" ::: "memory");
    #define __SFENCE__ __asm__ volatile ("DMB ISHST" ::: "memory");
#endif