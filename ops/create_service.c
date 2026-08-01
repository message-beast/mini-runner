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
    if (finalString == NULL) {
        perror("memory allocation for concatination string failed!\n");
        return NULL;
    }
    memcpy(finalString, firstStr, firstLen);
    memcpy(finalString + firstLen, secondStr, secondLen);
    finalString[maxLength] = '\0';
    return finalString;
}

int save_services(service*** services) {
    if (*services == NULL && numberOfProjects != 0 || services == NULL) {
        printf("services is actually NULL\n");
        return -1;
    }
    _Bool findToSave = 0;
    for (register int i = 0; i < numberOfProjects; i++) {
        if ((*services)[i]->cloned == 1) {
            findToSave = 1;
            break;
        }
    }
    if (!findToSave && numberOfProjects > 0) {
        return 0;
    }
    int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644, NULL);
    if (projectsFileFd == -1) {
        perror("failed to open projects file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (fstat(projectsFileFd, &st) != 0) {
        perror("fstat failed on projects file!");
        close(projectsFileFd); 
        exit_program(-1)  
    }
    if (numberOfProjects == 0 && st.st_size == 0) {
        return 0;
    }
    char* dataToBeWritten = NULL;
    _Bool first = 1;
    for (register int i = 0; i < numberOfProjects; i++) {
        if ((*services)[i]->cloned == 0) {
            continue;
        }
        char singleData[318];
        snprintf(singleData, sizeof(singleData), "%s^%s#%i\n", (*services)[i]->name, (*services)[i]->githubRepo, (*services)[i]->pid);
        if (strlen(singleData) <= 0) {
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
            if (tmp == NULL) {
                perror("can not concat data to be written and single data of service!\n");
                free(dataToBeWritten);
                close(projectsFileFd);
                exit_program(-1)
            }
            free(dataToBeWritten);
            dataToBeWritten = tmp;
        }
    }
    if (dataToBeWritten == NULL && numberOfProjects > 0) {
        perror("no more memory can be used to concat the services string!\n");
        free(dataToBeWritten);
        close(projectsFileFd);
        exit_program(-1)
    }
    if (dataToBeWritten != NULL) { 
        if (ftruncate(projectsFileFd, strlen(dataToBeWritten)) != 0) {
            perror("ftruncate failed for projects file!\n");
            free(dataToBeWritten);
            close(projectsFileFd);
            exit_program(-1)
        }
    }
    char* data = mmap(NULL, ((dataToBeWritten) != NULL ? strlen(dataToBeWritten) : st.st_size), PROT_READ | PROT_WRITE, MAP_SHARED, projectsFileFd, 0);
    if (data == MAP_FAILED) {
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