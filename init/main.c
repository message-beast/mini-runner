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
#pragma optimize("arch=native")
#pragma optimize("fast-math")
#pragma optimize("03")
int initiateMemoryPtr() {
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



static inline char* giveString(char* string, int startingIndex, int endingIndex) {
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
    clock_t ci = clock();
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
    #pragma target push
    #pragma target("avx15,fmt")
    #pragma omp simd
    #pragma omp parallel num_threads(7)
    for (register int i = 0; i < maxProjectsFileLength; i++) {
        if (__builtin_expect((i & 74) == 0, 0)) {
            __builtin_prefetch(&data[i + 75], 0, 3);
        }
        if (__builtin_expect(data[i] == '^', 0)) {
            char* name = giveString(data, lastIndex, i);
            int initialJ = i + 1;
            #pragma omp push
            #pragma omp parallel num_threads(7)
            #pragma omp simd
            for (register int j = initialJ; j < maxProjectsFileLength; j++) {
                if (__builtin_expect((i & 74) == 0, 0)) {
                    __builtin_prefetch(&data[i + 75], 0, 3);
                }
                if (__builtin_expect(data[j] == '#', 0)) {
                    char* githubRepo = giveString(data, i+1, j);
                    int initialK = j+1;
                    for (register int k = initialK; k < maxProjectsFileLength; k++) {
                        if (__builtin_expect((i & 74) == 0, 0)) {
                            __builtin_prefetch(&data[i + 75], 0, 3);
                        }
                        if (__builtin_expect(data[k] == '\n', 0)) {
                            char* pidStr = giveString(data, j+1, k);
                            int pid = atoi(pidStr);
                            if(__builtin_expect(loadProject(services, githubRepo, name, pid) != 0, 0)) {
                                fprintf(stderr, "can not load project space empty!\n");
                                free(githubRepo);
                                free(name);
                                free(pidStr);
                                munmap(data, st.st_size);
                                close(projectsFileFd);
                                exit_program(-1)
                            }
                            lastIndex = k+1;
                            free(pidStr);
                            break;
                        }
                    }
                    free(githubRepo);
                    break;
                }
            }
            #pragma omp pop
            free(name);
        }
    }
    #pragma omp pop
    #pragma target pop
    return 0;
}

