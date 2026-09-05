#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../basic.h"
#include "load_env.h"

#define ENV_FILE "data/env"

static inline __attribute__((always_inline, hot, aligned(64))) char* giveString(char* string, int startingIndex, int endingIndex) {
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

int loadEnvs(env*** __env) {
    if (__builtin_expect(__env == NULL, 0)) {
        env** tmp = malloc(sizeof(env*) * __INITIAL_SCALE_OF_ENV__);
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("failed to allocate memory for environment variables!\n");
            return -1;
        }
        (*__env) = tmp;
    }
    int fd = open(ENV_FILE, O_CREAT, O_RDONLY, 0644);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open env file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on the env file!\n");
        return -1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failedon env file!\n");
        return -1;
    }
    enum {FIND_NAME, FIND_KEY, FIND_VALUE} state = FIND_NAME;
    int lastIndex = 0;
    char* name = NULL;
    char* key = NULL;
    for (register int i = 0; i < st.st_size; ++i) {
        switch(state) {
            case FIND_NAME:
                if (__builtin_expect(data[i] == '`', 0)) {
                    char* __name = giveString(data, lastIndex, i);
                    if (__builtin_expect(__name == NULL, 0)) {
                        perror("failed to get name!\n");
                        exit_program(-1)
                    }
                    state = FIND_KEY;
                    lastIndex = i + 1;
                    name = __name;
                }
                break;
            case FIND_KEY:
                if (__builtin_expect(data[i] == '^', 0)) {
                    char* __key = giveString(data, lastIndex, i);
                    if (__builtin_expect(__key == NULL, 0)) {
                        perror("failed to get key!\n");
                        exit_program(-1)
                    }
                    state = FIND_VALUE;
                    lastIndex = i + 1;
                    key = __key;
                }
                break;
            case FIND_VALUE:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    char* value = giveString(data, lastIndex, i);
                    if (__builtin_expect(value == NULL, 0)) {
                        perror("failed toget a value!\n");
                        exit_program(-1)
                    }
                    state = FIND_NAME;
                    lastIndex = i + 1;
                    env* newEnv = malloc(sizeof(env));
                    if (__builtin_expect(newEnv == NULL, 0)) {
                        perror("can't allocate memory for loaded env!\n");
                        exit_program(-1)
                    }
                    newEnv->key = key;
                    newEnv->name = name;
                    newEnv->value = value;
                    if (__builtin_expect(loadEnv(__env, newEnv) != 0, 0)) {
                        perror("can't load env!\n");
                        exit_program(-1)
                    }
                }
                break;
        }
    }
    return 0;
}