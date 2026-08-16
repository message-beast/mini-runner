#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../base/structure.h"
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





__attribute__((hot)) float numOfCores(char* __restrict__ data, char* __restrict__ uptimeData) {
    int lastIndex = 0;
    int utime = 0;
    int stime = 0;
    int startTime = 0;
    int nowInJiffies = 0;
    int dataLen = strlen(data);
    int foundVars = 0;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    for (register int i = 0; i < dataLen; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&data[i + 128], 0, 3);
        }
        if (__builtin_expect(data[i] == ' ', 0)) {
            if (lastIndex == 13) {
                char* value = giveString(data, 14, i);
                if (__builtin_expect(value == NULL, 0)) {
                    perror("failed to create utime!\n");
                    exit_program(1)
                }
                foundVars++;
                utime = atoi(value);
                free(value);
            } else if (lastIndex == 14) {
                char* value = giveString(data, 15, i);
                if (__builtin_expect(value == NULL, 0)) {
                    perror("failed to create stime!\n");
                    exit_program(1)
                }
                foundVars++;
                stime = atoi(value);
                free(value);
            } else if (lastIndex == 21) {
                char* value = giveString(data, 22, i);
                if (__builtin_expect(value == NULL, 0)) {
                    perror("failed to create starttime!\n");
                    exit_program(1)
                }
                foundVars++;
                startTime = atoi(value);
                free(value);
            }
            lastIndex++;
        }
    }
    int timeLen = strlen(uptimeData);
    if (__builtin_expect(timeLen <= 0, 0)) {
        perror("failed to calculate timeLen!\n");
        exit_program(1)
    }
    for (register int i = 0; i < timeLen; ++i) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&uptimeData[i + 64], 0, 3);
        }
        if (__builtin_expect(uptimeData[i] == ' ', 0)) {
            char* value = giveString(uptimeData, 0, i);
            if (__builtin_expect(value == NULL, 0)) {
                perror("failed to create value for uptime!\n");
                exit_program(1)
            }
            foundVars++;
            nowInJiffies = atoi(value) * 100;
            free(value);
        }
    }
    if (__builtin_expect(foundVars < 4, 0)) {
        printf("\033[33mWarning cpu cpu data is not fully parsed!\033[0m\n");
    }
    if (__builtin_expect(nowInJiffies - startTime <= 0, 0)) {
        return 0.0f;
    }
    float cpuCore = (float)(utime + stime) / (nowInJiffies - startTime);
    return cpuCore;
}