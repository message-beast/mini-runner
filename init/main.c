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
int initiateMemoryPtr() {
    int sharedUpdateStatusFileFd = open("data/updateStatus", O_CREAT | O_RDWR, 0644, NULL);
    if (sharedUpdateStatusFileFd == -1) {
        perror("can not open the update status file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (fstat(sharedUpdateStatusFileFd, &st) != 0) {
        perror("fstat failed!\n");
        exit_program(-1)
    }

    if (ftruncate(sharedUpdateStatusFileFd, strlen("idle")) != 0) {
        printf("ftruncate failed!\n");
        exit_program(-1)
    }
    char* data = mmap(NULL, strlen("idle"), PROT_READ | PROT_WRITE, MAP_SHARED, sharedUpdateStatusFileFd, 0);
    if (data == MAP_FAILED) {
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
    if (finalString == NULL) {
        perror("can not allocate memory for the string");
        return NULL;
    }
    memcpy(finalString, string + startingIndex, length);
    finalString[length] = '\0';
    return finalString;
}



int loadServices(service*** services) {
    if (services == NULL) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (tmp == NULL) {
            perror("memory allocation for services failed!");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644, NULL);
    if (projectsFileFd == -1) {
        perror("can not open the projects file!\n");
        return -1;
    }
    struct stat st;
    if (fstat(projectsFileFd, &st) != 0) {
        perror("fstat for projects file failed!\n");
        close(projectsFileFd);
        return -1;
    }
    if (st.st_size == 0) {
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, projectsFileFd, 0);
    if (data == MAP_FAILED) {
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
    for (register int i = 0; i < maxProjectsFileLength; i++) {
        if (data[i] == '^') {
            char* name = giveString(data, lastIndex, i);
            int initialJ = i + 1;
            for (register int j = initialJ; j < maxProjectsFileLength; j++) {
                if (data[j] == '#') {
                    char* githubRepo = giveString(data, i+1, j);
                    int initialK = j+1;
                    for (register int k = initialK; k < maxProjectsFileLength; k++) {
                        if (data[k] == '\n') {
                            char* pidStr = giveString(data, j+1, k);
                            int pid = atoi(pidStr);
                            if(loadProject(services, githubRepo, name, pid) != 0) {
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
            free(name);
        }
    }
    return 0;
}

