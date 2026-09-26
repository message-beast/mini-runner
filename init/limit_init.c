#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "../basic.h"
#include <stdlib.h>
#include "../utils_ops/load_limit.h"

#define FILE "data/limits"

static inline __attribute__((always_inline, hot, aligned(64), malloc)) char* giveString(char* string, int startingIndex, int endingIndex) {
    int length = endingIndex - startingIndex;
    char* finalString = malloc(length + 1);
    if (__builtin_expect(finalString == NULL, 0)) {
        perror("can not allocate memory for the string");
        return NULL;
    }
    memcpy(finalString, string + startingIndex, length);
    finalString[length] = '\0';
    return finalString;
}

__attribute__((hot)) int initLimits(limit*** limits) {
    int fd = open(FILE, O_CREAT | O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open limit files!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on limits file!\n");
        close(fd);
        return -1;
    }
    int size = st.st_size;
    char* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("failed to map limit file!\n");
        close(fd);
        return -1;
    }
    enum {FIND_NAME, FIND_MEM_LIMIT, FIND_CPU_LIMIT} state = FIND_NAME;
    char* name = NULL;
    __uint64_t cpu = 0;
    __uint128_t memory = 0;
    int lastIndex = 0;
    for (register int i = 0; i < size; ++i) {
        switch(state) {
            case FIND_NAME:
                if (__builtin_expect(data[i] == '^', 0)) {
                    char* tmp = giveString(data, lastIndex, i);
                    if (__builtin_expect(tmp == NULL, 0)) {
                        perror("failed to get name from limits!\n");
                        goto cleanup;
                    }
                    name = tmp;
                    state = FIND_MEM_LIMIT;
                    lastIndex = i + 1;
                }
                break;
            case FIND_MEM_LIMIT:
                if (__builtin_expect(data[i] == '#', 0)) {
                    char* memLimitBuff = giveString(data, lastIndex, i);
                    if (__builtin_expect(memLimitBuff == NULL, 0)) {
                        perror("can't allocte memory for memory buffer from limit!\n");
                        free(name);
                        goto cleanup;
                    }
                    __uint128_t memLimit = atoll(memLimitBuff);
                    free(memLimitBuff);
                    if (__builtin_expect(memLimit == 0, 0)) {
                        fprintf(stderr, "failed to parse memory limit buffer from limit!\n");
                        free(name);
                        goto cleanup;
                    }
                    memory = memLimit;
                    state = FIND_CPU_LIMIT;
                    lastIndex = i + 1;
                }
                break;
            case FIND_CPU_LIMIT:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    char* cpuLimitBuff = giveString(data, lastIndex, i);
                    if (__Builtin_expect(cpuLimitBuff == NULL, 0)) {
                        perror("failed to allocate memory for ");
                        free(name);
                        goto cleanup;
                    }
                    __uint64_t cpuLimit = atol(cpuLimitBuff);
                    free(cpuLimit);
                    if (__builtin_expect(cpuLimit == 0, 0)) {
                        fprintf(stderr, "failed to parse limit cpu buffer!\n");
                        free(name);
                        goto cleanup;
                    }
                    cpu = cpuLimit;
                    
                    limit* newLimit = malloc(sizeof(limit));
                    if (__builtin_expect(newLimit == NULL, 0)) {
                        perror("failed to allocate memory for new limit!\n");
                        free(name);
                        goto cleanup;
                    }
                    newLimit->name = name;
                    newLimit->memory = memory;
                    newLimit->cpu = cpu;
                    if (__builtin_expect(loadLimit(limits, newLimit) != 0, 0)) {
                        free(name);
                        munmap(data, size);
                        close(fd);
                        return -1;
                    }
                    state = FIND_NAME;
                    lastIndex = i + 1;
                    name = NULL;
                }
                break;
        }
    }
    cleanup:
        munmap(data, size);
        close(fd);
        exit_program(-1)
    munmap(data, size);
    close(fd);
    return 0;
}