#ifndef ARM_SPECS_H
    #define ARM_SPECS_H
#endif

#include "arch.h"

#define X86_CPU 88
#define ARM_CPU 99

#if ARCH == ARM_32 || ARCH == ARM_64
    #define CPU_ARCH ARM_CPU
#elif ARCH == X86_64 || X86_32
    #define CPU_ARCH X86_CPU
#endif


#if CPU_ARCH == ARM_CPU
    #define __DSB_ISH__ __asm__ volatile ("dsb ish" ::: "memory");
    #define __DSB_ISHST__ __asm__ volatile ("dsb ishst" ::: "memory");
    #define __DSB_ISHLD__ __asm__ volatile ("dsb ishld" ::: "memory");
#else
    #define __DSB_ISH__
    #define __DSB_ISHST__
    #define __DSB_ISHLD__
#endif