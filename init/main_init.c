#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
#define _POSIX_C_SOURCE 200809L
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../basic.h"
#include <string.h>
#include <sys/stat.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "../utils.h"
#include <stdio.h>
//#define DEBUG_MODE 1
__attribute__((hot)) int initiateMemoryPtr() {
    int sharedUpdateStatusFileFd = open("data/updateStatus", O_CREAT | O_RDWR, 0644, NULL);
    if (__builtin_expect(sharedUpdateStatusFileFd == -1, 0)) {
        perror("can not open the update status file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(sharedUpdateStatusFileFd, &st) != 0, 0)) {
        perror("fstat failed!\n");
        exit_program(-1)
    }

    if (__builtin_expect(ftruncate(sharedUpdateStatusFileFd, strlen("idle")) != 0, 0)) {
        printf("ftruncate failed!\n");
        exit_program(-1)
    }
    char* data = mmap(NULL, strlen("idle"), PROT_READ | PROT_WRITE, MAP_SHARED, sharedUpdateStatusFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("map failed for shared_update_file\n");
        exit_program(-1)
    }
    strcpy(data, "idle");
    msync(data, strlen("idle"), MS_ASYNC);
    munmap(data, strlen("idle"));
    close(sharedUpdateStatusFileFd);
    return 0;
}



static inline __attribute__((always_inline, hot)) char* giveString(char* string, int startingIndex, int endingIndex) {
    int length = endingIndex - startingIndex;
    char* finalString = calloc(length + 1, sizeof(char));
    if (__builtin_expect(finalString == NULL, 0)) {
        perror("can not allocate memory for the string");
        return NULL;
    }
    memcpy(finalString, string + startingIndex, length);
    finalString[length] = '\0';
    return finalString;
}



__attribute__((hot)) int loadServices(service*** services) {
    if (__builtin_expect(services == NULL, 0)) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("memory allocation for services failed!");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644, NULL);
    if (__builtin_expect(projectsFileFd == -1, 0)) {
        perror("can not open the projects file!\n");
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(projectsFileFd, &st) != 0, 0)) {
        perror("fstat for projects file failed!\n");
        close(projectsFileFd);
        return -1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        printf("hey!!\n");
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, projectsFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("map failed for projects file!\n");
        close(projectsFileFd);
        exit_program(-1)
    }
    int maxProjectsFileLength = st.st_size;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    int lastIndex = 0;
    char* name = NULL;
    char*githubRepo = NULL;
    enum { FIND_NAME, FIND_REPO, FIND_PID } state = FIND_NAME;
    for (register int i = 0; i < maxProjectsFileLength; i++) {
        if (__builtin_expect((i & 74) == 0, 0)) {
            __builtin_prefetch(&data[i + 75], 0, 3);
        }

        switch(state) {
            case FIND_NAME:
                if (__builtin_expect(data[i] == '^', 0)) {
                    name = giveString(data, lastIndex, i);
                    lastIndex = i + 1;
                    state = FIND_REPO;
                }
            case FIND_REPO:
                if (__builtin_expect(data[i] == '#', 0)) {
                    githubRepo = giveString(data, lastIndex, i);
                    lastIndex = i + 1;
                    state = FIND_PID;
                }
            case FIND_PID:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    char* pidStr = giveString(data, lastIndex, i);
                    int pid = atoi(pidStr);
                    if(__builtin_expect(loadProject(services, githubRepo, name, pid) != 0, 0)) {
                        fprintf(stderr, "can not load project space empty!\n");
                        free(githubRepo);
                        free(name);
                        free(pidStr);
                        name = NULL;
                        githubRepo = NULL;
                        munmap(data, st.st_size);
                        close(projectsFileFd);
                        exit_program(-1)
                    }
                    free(pidStr);
                    free(name);
                    free(githubRepo);
                    name = NULL;
                    githubRepo = NULL;
                    lastIndex = i + 1;
                    state = FIND_NAME;
                }
        }
    }
    return 0;
}

