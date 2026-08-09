#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "../basic.h"
#include <string.h>
static inline __attribute__((always_inline, hot)) char* concat(char* __restrict__ firstStr, char* __restrict__ secondStr) {
    size_t firstLen = strlen(firstStr);
    size_t secondLen = strlen(secondStr);
    int maxLength = firstLen+ secondLen;
    char* finalString = malloc(maxLength + 1);
    if (__builtin_expect(finalString == NULL, 0)) {
        perror("memory allocation for concatination string failed!\n");
        return NULL;
    }
    memcpy(finalString, firstStr, firstLen);
    memcpy(finalString + firstLen, secondStr, secondLen);
    finalString[maxLength] = '\0';
    return finalString;
}

__attribute__((hot)) int save_services(service*** services) {
    if (__builtin_expect(*services == NULL && numberOfProjects != 0 || services == NULL, 0)) {
        printf("services is actually NULL\n");
        return -1;
    }
    _Bool findToSave = 0;
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((*services)[i]->cloned == 1, 1)) {
            findToSave = 1;
            break;
        }
    }
    if (__builtin_expect(!findToSave && numberOfProjects > 0, 0)) {
        return 0;
    }
    int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644, NULL);
    if (__builtin_expect(projectsFileFd == -1, 0)) {
        perror("failed to open projects file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (__builtin_expect(fstat(projectsFileFd, &st) != 0, 0)) {
        perror("fstat failed on projects file!");
        close(projectsFileFd); 
        exit_program(-1)  
    }
    if (__builtin_expect(numberOfProjects == 0 && st.st_size == 0, 0)) {
        return 0;
    }
    char* dataToBeWritten = NULL;
    _Bool first = 1;
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((*services)[i]->cloned == 0, 0)) {
            continue;
        }
        char singleData[318];
        snprintf(singleData, sizeof(singleData), "%s^%s#%i\n", (*services)[i]->name, (*services)[i]->githubRepo, (*services)[i]->pid);
        if (__builtin_expect(strlen(singleData) <= 0, 0)) {
            perror("can not copy single service data!\n");
            free(dataToBeWritten);
            close(projectsFileFd);
            exit_program(-1)
        }
        if (__builtin_expect(first, 0)) {
            dataToBeWritten = strdup(singleData);
            first = 0;
        } else {
            char* tmp = concat(dataToBeWritten, singleData);
            if (__builtin_expect(tmp == NULL, 0)) {
                perror("can not concat data to be written and single data of service!\n");
                free(dataToBeWritten);
                close(projectsFileFd);
                exit_program(-1)
            }
            free(dataToBeWritten);
            dataToBeWritten = tmp;
        }
    }
    if (__builtin_expect(dataToBeWritten == NULL && numberOfProjects > 0, 0)) {
        perror("no more memory can be used to concat the services string!\n");
        free(dataToBeWritten);
        close(projectsFileFd);
        exit_program(-1)
    }
    if (__builtin_expect(dataToBeWritten != NULL, 0)) { 
        if (ftruncate(projectsFileFd, strlen(dataToBeWritten)) != 0) {
            perror("ftruncate failed for projects file!\n");
            free(dataToBeWritten);
            close(projectsFileFd);
            exit_program(-1)
        }
    }
    char* data = mmap(NULL, ((dataToBeWritten) != NULL ? strlen(dataToBeWritten) : st.st_size), PROT_READ | PROT_WRITE, MAP_SHARED, projectsFileFd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("memory mapping failed for projects file!\n");
        free(dataToBeWritten);
        close(projectsFileFd);
        exit_program(-1)
    }
    __asm__  volatile (
        "sfence"
        :
        :
        : "memory"
    );
    if (__builtin_expect(dataToBeWritten != NULL && numberOfProjects > 0, 1)) {
        memcpy(data, dataToBeWritten, strlen(dataToBeWritten));
        msync(data, strlen(dataToBeWritten), MS_SYNC);
    } else {
        ftruncate(projectsFileFd, 0);
    }
    munmap(data, (dataToBeWritten != NULL ? strlen(dataToBeWritten) : st.st_size));
    close(projectsFileFd);
    free(dataToBeWritten);
    return 0;
}