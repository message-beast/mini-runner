#pragma once
#include <stdlib.h>
#define DECLARE_128_T\
    static inline __attribute__((always_inline, hot)) char* uint128_to_chars(__uint128_t num) {\
    static char buffer[65];\
    char temp[65];\
    int i = 0, j = 0;\
    if (__builtin_expect(num == 0, 0)) {\
        buffer[0] = '0';\
        buffer[1] = '\0';\
        return buffer;\
    }\
    while (num > 0 && i < 64) {\
        temp[i++] = '0' + (num % 10);\
        num /= 10;\
    }\
    while (i > 0 && j < 64) {\
        buffer[j++] = temp[--i];\
    }\
    buffer[j] = '\0';\
    return buffer;\
}\


#define format_128_t(n) uint128_to_chars(n)


#define DECLARE_CHARS_128_T\
    static inline __attribute__((always_inline, hot)) __uint128_t chars_to_uint_128(char* numBuff) {\
        long long loaded = atoll(numBuff);\
        __uint128_t res = loaded;\
        return res;\
    }\


#define format_128_char(x) chars_to_uint_128(x);


#define CHECK_WRITE(fd, _w, len)\
    if (__builtin_expect(_w <= 0 || _w != len, 0)) {\
        perror("write failed!\n");\
        close(fd);\
        return -1;\
    }

#define invalidString(x) strlen(x) <= 0


