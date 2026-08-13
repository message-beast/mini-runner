#pragma optimize("O3")
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


__attribute__((hot)) int convertToByte(char* size) {
    int size_len = strlen(size);
    int memBytes = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < size_len; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&size[i + 128], 0, 3);
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