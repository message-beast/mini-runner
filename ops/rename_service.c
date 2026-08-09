#pragma optimize("03")
#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static inline __attribute__((always_inline, hot)) int renameServiceProjectFoler(char* __restrict__ beforeName, char* __restrict__ newName) {
    size_t size = snprintf(NULL, 0, "/var/lib/%s", beforeName);
    size_t newSize = snprintf(NULL, 0, "/var/lib/%s", newName);
    if (__builtin_expect(size <= 0 || newSize <= 0, 0)) {
        perror("faile to calculate the size of the pathof old/new directory");
        return -1;
    }
    char beforeFolderName[size + 1];
    char newFolderName[newSize + 1];
    snprintf(beforeFolderName, size + 1, "/var/lib/%s", beforeName);
    snprintf(newFolderName, newSize + 1, "/var/lib/%s", newName);
    if (__builtin_expect(beforeFolderName == NULL || newFolderName == NULL, 0)) {
        perror("failed to get the string of full path of old/new project directory\n");
        return -1;
    }
    if (__builtin_expect(rename(beforeFolderName, newFolderName) != 0, 0)) {
        if (errno == ENOENT) {
            fprintf(stderr, "\033[31mcan not find service with folder of /var/lib/%s\n", beforeFolderName);
        } else if (errno == EACCES) {
            fprintf(stderr, "\033[31mpermission denied! \033[33m may be rerun with sudo\033[0m\n");
        } else if (errno == EXDEV) {
            fprintf(stderr, "\033[31mcross-device move is not supported!\n");
        }
        return -1;
    }
    return 0;

}



void renameService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ newName) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        return;
    }
    if (__builtin_expect(serviceName == NULL || newName == NULL, 0)) {
        fprintf(stderr, "\033[31mservice name or new service name not provided!\033[0m\n");
        return;
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);   
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            char* beforeName = currentService->name;
            if (__builtin_expect(renameServiceProjectFoler(serviceName, newName) != 0, 0)) {
                beforeName = NULL;
                return;
            }
            currentService->name = strdup(newName);
            printf("\033[33mservice \033[31m\"%s\" \033[33msuccessfully renamed to \033[32m\"%s\"\033[0m\n", beforeName, currentService->name);
            free(beforeName);
            beforeName = NULL;
            return;
        }
    }
    fprintf(stderr, "can not find any service with name %s\n", serviceName);
    return;
}