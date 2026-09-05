#include <stdio.h>
#include <string.h>
#include "../../base/structure.h"
#include <stdlib.h>
#include "../../base/config.h"
#include "../utils/free_mrn_execs.h"
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



mrnExec* getParse(char* __command, int len) {
    int lastIndex = 0;
    int found = 0;
    printf("recieving: %s\nlen: %i\n", __command, len);
    mrnExec* mrn_exec = malloc(sizeof(mrnExec));
    mrn_exec->commands = malloc(sizeof(char*) * __INITIAL_SCALE_NUM_ARGS__);
    mrn_exec->capacity = __INITIAL_SCALE_NUM_ARGS__;
    mrn_exec->next = NULL;
    mrn_exec->numbers = 0;
    for (register int i = 0; i < len; ++i) {
        if (__builtin_expect((i & 512) == 0 || i == 0, 0)) {
            __builtin_prefetch(&__command[i + 512], 0, 3);
        }
        if (__builtin_expect(__command[i] == ' ', 0)) {
            char* word = giveString(__command, lastIndex, i);
            if (__builtin_expect(word == NULL, 0)) {
                free(mrn_exec->commands);
                free(mrn_exec);
                return NULL;
            }
            if (__builtin_expect(found >= mrn_exec->capacity, 0)) {
                int newCapacity = sizeof(char*) * (mrn_exec->capacity + __INITIAL_SCALE_NUM_ARGS__);
                char** tmp = realloc(mrn_exec->commands, newCapacity);
                if (__builtin_expect(tmp == NULL, 0)) {
                    perror("failed to reallocate memory for args!\n");
                    for (register int j = 0; j < found; ++j) {
                        free(mrn_exec->commands[j]);
                    }
                    free(mrn_exec->commands);
                    free(mrn_exec);
                    return NULL;
                }
                mrn_exec->capacity = newCapacity;
            }
            printf("inserting: %s to %i index\n", word, found);
            mrn_exec->commands[found] = word;
            lastIndex = i + 1;
            found++;
        } else if (__builtin_expect(i == len - 1, 0)) {
            char* word = giveString(__command, lastIndex, len);
            if (__builtin_expect(word == NULL, 0)) {
                for (register int j = 0; j < found; ++j) {
                    free(mrn_exec->commands[j]);
                }
                free(mrn_exec->commands);
                free(mrn_exec);
                return NULL;
            }
            if (__builtin_expect(found >= mrn_exec->capacity, 0)) {
                int newCapacity = sizeof(char*) * (mrn_exec->capacity + __INITIAL_SCALE_NUM_ARGS__);
                char** tmp = realloc(mrn_exec->commands, newCapacity);
                if (__builtin_expect(tmp == NULL, 0)) {
                    perror("failed to reallocate memory for args!\n");
                    for (register int j = 0; j < found; ++j) {
                        free(mrn_exec->commands[j]);
                    }
                    free(mrn_exec->commands);
                    free(mrn_exec);
                    return NULL;
                }
                mrn_exec->capacity = newCapacity;
            }
            printf("inserting: %s to %i index\n", word, found);
            mrn_exec->commands[found] = word;
            lastIndex = i + 1;
            found++;
        }
    }

    mrn_exec->commands[found++] = NULL;
    mrn_exec->numbers = found;
    mrn_exec->next = NULL;
    return mrn_exec;
}