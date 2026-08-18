#pragma optimize("O3")
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../res_man/utils/helper.h"

DECLARE_128_T


__attribute__((hot)) __uint128_t convertToByte_F_EXTR(char* size) {
    int size_len = strlen(size);
    __uint128_t memBytes = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < size_len; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&size[i + 64], 0, 3);
        }
        if (__builtin_expect(size[i] == 't', 0)) {
            char memSize[i + 2];
            memcpy(memSize, size, i);
            memSize[i] = '\0';
            int memSizeInt = atoi(memSize);
            memBytes = (__uint128_t)memSizeInt * (__uint64_t)1024 * 1024 * 1024 * 1024;
            break;
        }
    }
    return memBytes != 0 ? memBytes : atoi(size);
}



__attribute__((hot)) __uint64_t convertToByte_F_LRG(char* size) {
    int size_len = strlen(size);
    __uint64_t memBytes = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < size_len; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&size[i + 64], 0, 3);
        }
        if (__builtin_expect(size[i] == 'g', 0)) {
            char memSize[i + 2];
            memcpy(memSize, size, i);
            memSize[i] = '\0';
            int memSizeInt = atoi(memSize);
            memBytes = (__uint64_t)memSizeInt * (__uint32_t)1024 * 1024 * 1024;
            break;
        }
    }
    return memBytes != 0 ? memBytes : atoi(size);
}





__attribute__((hot)) int convertToByte(char* size) {
    int size_len = strlen(size);
    int memBytes = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < size_len; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&size[i + 64], 0, 3);
        }
        if (__builtin_expect(size[i] == 'k', 0)) {
            char memSize[i + 2];
            memcpy(memSize, size, i);
            memSize[i] = '\0';
            int memSizeInt = atoi(memSize);
            memBytes = memSizeInt * 1024;
            break;
        } else if (__builtin_expect(size[i] == 'm', 0)) {
            char memSize[i + 2];
            memcpy(memSize, size, i);
            memSize[i] = '\0';
            int memSizeInt = atoi(memSize);
            memBytes = memSizeInt * (1024 * 1024);
            break;
        } else if (__builtin_expect(size[i] == 'g', 0)) {
            char memSize[i + 2];
            memcpy(memSize, size, i);
            memSize[i] = '\0';
            int memSizeInt = atoi(memSize);
            memBytes = memSizeInt * (1024 * 1024 * 1024);
            break;
        }
    }
    return memBytes != 0 ? memBytes : atoi(size);
}


__attribute__((hot)) char* formatByte(__uint128_t bytes) {
    char* formatedString = NULL;
    if (((float)bytes / ((__uint64_t) 1024 * 1024 * 1024* 1024)) >= 1) {
        float amount = (float)bytes / ((__uint64_t) 1024 * 1024 * 1024* 1024);
        size_t size = snprintf(NULL, 0, "%2.f TB", amount);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate size!\n");
            return NULL;
        }
        formatedString = malloc(size + 1);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, size + 1, "%2.f TB", amount);
        formatedString[size] = '\0';
    } else if (((float) bytes / ((__uint32_t) 1024 * 1024 * 1024)) >= 1) {
        float amount = (float) bytes / ((__uint64_t) 1024 * 1024 * 1024);
        size_t size = snprintf(NULL, 0, "%.2f GB", amount);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate size!\n");
            return NULL;
        }
        formatedString = malloc(size + 1);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, size + 1, "%.2f GB", amount);
        formatedString[size] = '\0';
    } else if (((float) bytes / ((__uint32_t) 1024 * 1024)) >= 1) {
        float amount = (float) bytes / ((__uint64_t) 1024 * 1024);
        size_t size = snprintf(NULL, 0, "%.2f MB", amount);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate size!\n");
            return NULL;
        }
        formatedString = malloc(size + 1);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, size + 1, "%.2f MB", amount);
        formatedString[size] = '\0';
    } else if (((float) bytes / 1024) >= 1) {
        float amount = (float) bytes / 1024;
        size_t size = snprintf(NULL, 0, "%.2f KB", amount);
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate size!\n");
            return NULL;
        }
        formatedString = malloc(size + 1);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, size + 1, "%.2f KB", amount);
        formatedString[size] = '\0';
    } else if (bytes > 0) {
        size_t size = snprintf(NULL, 0, "%s BYTES", format_128_t(bytes));
        if (__builtin_expect(size <= 0, 0)) {
            perror("failed to calculate size!\n");
            return NULL;
        }
        formatedString = malloc(size + 1);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, size + 1, "%s BYTES", format_128_t(bytes));
        formatedString[size] = '\0';
    } else {
        formatedString = malloc(4);
        if (__builtin_expect(formatedString == NULL, 0)) {
            perror("failed to allocate memory\n");
            return NULL;
        }
        snprintf(formatedString, 4, "N/A");
        formatedString[3] = '\0';
    }
    return formatedString;
}


