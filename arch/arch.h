#define X86_64 64
#define X86_32 32
#define ARM_64 53
#define ARM_32 57

#if defined(__x86_64__) || defined(_M_X64_)
    #define ARCH X86_64
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH X86_32
#elif defined(__aarch_64__) || defined(_M_ARM64)
    #define ARCH ARM_64
#elif defined(__arm__) || defined(__M_ARM)
    #define ARCH ARM_32

#endif