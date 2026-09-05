#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../../base/config.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "../../base/structure.h"
#include "../parser/parse.h"
#include "../utils/free_mrn_execs.h"
#include <stdlib.h>
static inline __attribute__((always_inline, hot, aligned(64))) int runCode(const mrnExec* __exec) {
    char* binary = __exec->commands[0];
    int pid = fork();
    if (pid == 0) {
        printf("\n");
        execvp(binary, (void*)__exec->commands);
        fprintf(stderr, "failed to execute \033[33m%s\033[0m\n", binary);
        abort();
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 0;
}


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

static inline __attribute__((always_inline, hot)) int ch(const char* __path) {
    if (__builtin_expect(chdir(__path) != 0, 0)) {
        perror("failed to change directory!\n");
        return -1;
    }
    return 0;
}


__attribute__((hot)) int runMrnFile(const char* __file) {
    int fileSize = 0;
    int fd = open(__file, O_RDONLY);
    if (__builtin_expect(fd == -1, 0)) {
        perror("failed to open mrn file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on mrn file!\n");
        return -1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        printf("file is empty!\n");
        return -1;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on the file!\n");
        return -1;
    }
    int endIndex = st.st_size - 1;
    int lastIndex = 0;
    int afterEnd = st.st_size + 1;
    mrnExec* mrn_execs = malloc(sizeof(mrnExec));
    if (__builtin_expect(mrn_execs == NULL, 0)) {
        perror("failed to allocate memory for mrn exec run time data!\n");
        return -1;
    }
    mrn_execs->next = NULL;
    mrn_execs->capacity = __INITIAL_SCALE_OF_ENV__;
    mrn_execs->numbers = 0;
    mrn_execs->commands = NULL;
    pid_t pid = fork();
    if (pid == 0) {
        for (register int i = 0; i < st.st_size; ++i) {
            if (__builtin_expect((i & 512) == 0 || i == 0, 0)) {
                __builtin_prefetch(&data[i + 512], 0, 3);
            }
            if (__builtin_expect(data[i] == '\n', 0)) {
                char* command = giveString(data, lastIndex, i);
                if (__builtin_expect(command == NULL, 0)) {
                    perror("failed to get command!\n");
                    abort();
                }
                if (__builtin_expect(command[0] == 'c' && command[1] == 'd', 0)) {
                    int size = strlen(command);
                    char* path = giveString(command, 3, size);
                    free(command);
                    if (__builtin_expect(ch(path) != 0, 0)) {
                        free(path);
                        abort();
                    }
                    free(path);
                } else {
                    //parse to mrn_exec
                    mrnExec* new_mrn_exec = getParse(command, strlen(command));
                    //pass to run code
                    if (__builtin_expect(new_mrn_exec == NULL, 0)) {
                        freeMrnExecs(&mrn_execs);
                        fprintf(stderr, "\033[31mfailed to execute \033[33m%s\033[0m\n", command);
                        free(command);
                        munmap(data, st.st_size);
                        close(fd);
                        abort();
                    }
                    new_mrn_exec->next = mrn_execs;
                    mrn_execs = new_mrn_exec;
                    if (__builtin_expect(runCode(new_mrn_exec) != 0, 0)) {
                        free(command);
                        munmap(data, st.st_size);
                        close(fd);
                        abort();
                    }
                    free(command);
                    command = NULL;
                }
                lastIndex = i + 1;
            } else if (__builtin_expect(i == endIndex, 0)) {
                char* command = giveString(data, lastIndex, afterEnd);
                if (__builtin_expect(command == NULL, 0)) {
                    perror("failed to get command!\n");
                    abort();
                }
                if (__builtin_expect(command[0] == 'c' && command[1] == 'd', 0)) {
                    int size = strlen(command);
                    char* path = giveString(command, 3, size);
                    free(command);
                    if (__builtin_expect(ch(path) != 0, 0)) {
                        free(path);
                        abort();
                    }
                    free(path);
                } else {
                    mrnExec* new_mrn_exec = getParse(command, strlen(command));
                    //pass to run code
                    if (__builtin_expect(new_mrn_exec == NULL, 0)) {
                        freeMrnExecs(&mrn_execs);
                        fprintf(stderr, "\033[31mfailed to execute \033[33m%s\033[0m\n", command);
                        munmap(data, st.st_size);
                        close(fd);
                        abort();
                    }
                    new_mrn_exec->next = mrn_execs;
                    mrn_execs = new_mrn_exec;
                    if (__builtin_expect(runCode(new_mrn_exec) != 0, 0)) {
                        freeMrnExecs(&mrn_execs);
                        free(command);
                        munmap(data, st.st_size);
                        close(fd);
                        abort();
                    }
                    free(command);
                    command = NULL;
                }
            }
        }
        munmap(data, st.st_size);
        close(fd);
        freeMrnExecs(&mrn_execs);
        abort();
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    freeMrnExecs(&mrn_execs);
    munmap(data, st.st_size);
    close(fd);
    return 0;
}