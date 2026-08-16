#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../../../base/structure.h"
#include <stdlib.h>
#include "../../../basic.h"

static inline __attribute__((always_inline, hot)) char* giveString(char* string, int startIndex, int endingIndex) {
    int length = endingIndex - startIndex;
    char* newString = malloc(length + 1);
    if (__builtin_expect(newString == NULL, 0)) {
        perror("failed to allocate memory for the cutted string!\n");
        return NULL;
    }
    memcpy(newString, string + startIndex, length);
    newString[length] = '\0';
    return newString;
}



enum Things {stack, heap, ram, disk, threads, unwanted};


static inline __attribute__((always_inline, hot)) enum Things isUseFull(char* name) {
    if (__builtin_expect(strcmp(name, "VmStk") == 0, 0)) {
        return stack;
    } else if (__builtin_expect(strcmp(name, "VmData") == 0, 0)) {
        return heap;
    } else if (__builtin_expect(strcmp(name, "VmSwap") == 0, 0)) {
        return disk;
    } else if (__builtin_expect(strcmp(name, "VmRSS") == 0, 0)) {
        return  ram;
    } else if (__builtin_expect(strcmp(name, "Threads") == 0, 0)) {
        return threads;
    } else {
        return unwanted;
    }
    return unwanted;
}



static inline __attribute__((always_inline, hot)) int giveStart(char* data, int index) {
    int dataLen = strlen(data);
    if (__builtin_expect(dataLen == 0, 0)) {
        perror("failed to calculate data length!\n");
        exit_program(1)
    }
    for (register int i = index; i < dataLen; ++i) {
        if (__builtin_expect(data[i] != ' ' && data[i] != '\t', 0)) {
            return index + (i - index);
        }
    }
    return index;
} 



__attribute__((hot)) meminfo* memoryParse(char* data) {
    enum {FIND_NAME, FIND_VALUE} state = FIND_NAME;
    int dataLen = strlen(data);
    if (__builtin_expect(dataLen == 0, 0)) {
        perror("can not open /proc/<ser pid>/status file!\n");
        exit_program(1)
    }
    meminfo* memResult = malloc(sizeof(meminfo));
    if (__builtin_expect(memResult == NULL, 0)) {
        perror("failed to allocate memory info heap!\n");
        return NULL;
    }
    memResult->disk = NULL;
    memResult->heap = NULL;
    memResult->stack = NULL;
    memResult->vmrss = NULL;
    memResult->numThreads = 0;
    int lastIndex = 0;
    enum Things currentThing = unwanted;
    for (register int i = 0; i < dataLen; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&data[i + 64]);
        }
        switch(state) {
            case FIND_NAME:
                if (__builtin_expect(data[i] == ':', 0)) {
                    char* name = giveString(data, lastIndex, i);
                    if (__builtin_expect(name == NULL, 0)) {
                        perror("can not allocate memory!");
                        exit_program(1)
                    }
                    if (__builtin_expect(isUseFull(name) != unwanted, 0)) {
                        currentThing = isUseFull(name);
                        lastIndex = i + 1;
                        state = FIND_VALUE;
                    }
                    free(name);
                } else if (__builtin_expect(data[i] == '\n', 0)) {
                    lastIndex = i + 1;
                }
                break;
            case FIND_VALUE:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    int startingIndex = giveStart(data, lastIndex);
                    char* value = giveString(data, startingIndex, i);
                    if (__builtin_expect(value == NULL, 0)) {
                        perror("failed to get value!\n");
                        exit_program(1)
                    }
                    if (currentThing == stack) {
                        memResult->stack = value;
                    } else if (currentThing == heap) {
                        memResult->heap = value;
                    } else if (currentThing == ram) {
                        memResult->vmrss = value;
                    } else if (currentThing == disk) {
                        memResult->disk = value;
                    } else if (currentThing == threads) {
                        memResult->numThreads = atoi(value);
                        free(value);
                    } else {
                        free(value);
                    }
                    lastIndex = i + 1;
                    state = FIND_NAME;
                }
                break;
        }
    }
    return memResult;
}