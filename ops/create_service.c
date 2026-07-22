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

static inline __attribute__((always_inline)) char* concat(char* __restrict__ firstStr, char* __restrict__ secondStr) {
    if (strlen(firstStr) != 0 && strlen(secondStr) == 0) {
        return firstStr;
    } else if (strlen(firstStr) == 0 && strlen(secondStr) != 0) {
        return secondStr;
    }
    int maxLength = strlen(firstStr) + strlen(secondStr);
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    STR char* finalString = calloc(maxLength, sizeof(char));
    if (finalString == NULL) {
        perror("memory allocation for concatination string failed!\n");
        return NULL;
    }

    for (register int i = 0; i < maxLength; i++) {
        if (i < strlen(firstStr)) {
            finalString[i] = firstStr[i];
        } else {
            finalString[i] = secondStr[i - strlen(firstStr)];
        }
    }
    return finalString;
}

int save_services(service** services) {
    _Bool findToSave = 0;
    for (register int i = 0; i < numberOfProjects; i++) {
        if (services[i]->cloned == 1) {
            findToSave = 1;
            break;
        }
    }
    if (!findToSave) {
        return 0;
    }
    int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644, NULL);
    if (projectsFileFd == -1) {
        perror("failed to open projects file!\n");
        exit_program(-1)
    }
    struct stat st;
    if (fstat(projectsFileFd, &st) != 0) {
        perror("fstat failed on projects file!\n");
        exit_program(-1)
    }
    char* dataToBeWritten = calloc(318, numberOfProjects);
    if (dataToBeWritten == NULL) {
        perror("can not allocate memory for the data to write!\n");
        exit_program(-1)
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (services[i]->cloned == 0) {
            continue;
        }
        char* singleData = calloc(318, sizeof(char));
        if (singleData == NULL) {
            perror("can not allocate memory to write single service data\n");
            free(dataToBeWritten);
            exit_program(-1)
        }
        snprintf(singleData, 318, "%s^%s#%i\n", services[i]->name, services[i]->githubRepo, services[i]->pid);
        if (strlen(singleData) <= 0) {
            perror("can not copy single service data!\n");
            exit_program(-1)
        }
        if (concat(dataToBeWritten, singleData) == NULL) {
            perror("can not concat data to be written and single data of service!\n");
            exit_program(-1)
        }
        dataToBeWritten = concat(dataToBeWritten, singleData);
        printf("datatobeWritten:\n%s\n", dataToBeWritten);
    }
    if (dataToBeWritten == NULL) {
        perror("no more memory can be used to concat the services string!\n");
        exit_program(-1)
    }
    if (ftruncate(projectsFileFd, strlen(dataToBeWritten)) != 0) {
        perror("ftruncate failed for projects file!\n");
        exit_program(-1)
    }
    char* data = mmap(NULL, strlen(dataToBeWritten), PROT_READ | PROT_WRITE, MAP_SHARED, projectsFileFd, 0);
    if (data == MAP_FAILED) {
        perror("memory mapping failed for projects file!\n");
        exit_program(-1)
    }  
    strcpy(data, dataToBeWritten);
    msync(data, strlen(dataToBeWritten), MS_ASYNC);
    munmap(data, strlen(dataToBeWritten));
    free(dataToBeWritten);
    close(projectsFileFd);
    return 0;
}