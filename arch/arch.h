#define X86_64 64
#define X86_32 32

#if defined(__x86_64__) || defined(_M_X64_)
    #define ARCH X86_64
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH X86_32
#endif