#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../base/structure.h"
#include "../basic.h"
#include <string.h>
#include "../base/config.h"
#include <sys/wait.h>
#include "../arch/opt.h"
#include "../file_sys_ops/file_system.h"
#include "../utils.h"
#include <signal.h>

static inline __attribute__((always_inline, hot)) char* cutString(char* __restrict__ data, int startingIndex, int endingIndex) {
    int dataLength = strlen(data);
    if (__builtin_expect(dataLength <= 0, 0)) {
        return NULL;
    }
    //for top removal
    if (startingIndex == 0) {
        char* newString = malloc((dataLength - endingIndex) * sizeof(char));
        if (__builtin_expect(newString == NULL, 0)) {
            perror("memory allocation for new string failed on top!\n");
            return NULL;
        }
        #pragma GCC unroll 4
        for (register int i = 0; i < dataLength - endingIndex - 1; i++) {
            if (__builtin_expect((i & 512) == 0 || i == 0, 0)) {
                __builtin_prefetch(&data[endingIndex + i + 513]);
            }
            newString[i] = data[endingIndex + i + 1];
        }
        newString[dataLength - endingIndex - 1] = '\0';

        return newString;

    }


    //for bottom removal
    if (endingIndex == dataLength - 1) {
        char* newString = malloc((startingIndex + 1) * sizeof(char));
        if (__builtin_expect(newString == NULL, 0)) {
            perror("memory allocaton for new string failed for the bottom remoal!\n");
            return NULL;
        }
        #pragma GCC ivdep
        #pragma GCC unroll 4
        for (register int i = 0; i < startingIndex; i++) {
            if (__builtin_expect((i & 511) == 0 || i == 0, 0)) {
                __builtin_prefetch(&data[i + 512], 0, 3);
            }
            newString[i] = data[i];
        }
        newString[startingIndex] = '\0';
        return newString;
    }

    // for middle removal
    int newLength  = dataLength - (endingIndex - startingIndex);
    char* newString = malloc((newLength) * sizeof(char));
    if (__builtin_expect(newString == NULL, 0)) {
        perror("memory allocation for new string failed!");
        return NULL;
    }

    #pragma omp simd
    for (register int i = 0; i < newLength - 1; i++) {
        if (__builtin_expect((i & 127) == 0 && i >= startingIndex, 1)) {
            __builtin_prefetch(&data[endingIndex + (i - startingIndex) + 129], 0, 3);
        } else if (__builtin_expect((i & 511) == 0 && i < startingIndex || i == 0, 0)) {
            __builtin_prefetch(&data[i + 512], 0, 3);
        }
        if (__builtin_expect(i >= startingIndex, 1)) {
            newString[i] = data[endingIndex + (i - startingIndex) + 1];
        } else {
            newString[i] = data[i];
        }
    }
    newString[newLength - 1] = '\0';
    return newString;
    // for middle removal end

}


static inline __attribute__((always_inline, hot)) char* getValue(char* __restrict__ data, int start, int end) {
    if (__builtin_expect(data == NULL || start < 0 || end <= 0, 0)) {
        fprintf(stderr, "bad parameters!\n");
        return NULL;
    }
    int maxdataLength = strlen(data);
    char* newString = malloc(end - start + 2);
    if (__builtin_expect(newString == NULL, 0)) {
        perror("memory allocation for newString to just get value failed!\n");
        exit_program(-1)
    }
    #pragma GCC unroll 4
    for (register int i = start; i <= end; i++) {
        if (__builtin_expect((i & 511) == 0 || i == 0, 0)) {
            __builtin_prefetch(&data[i + 512], 0, 3);
        }
        newString[i - start] = data[i];
    }
    newString[end - start + 1] = '\0';
    return newString;
}




static inline __attribute__((always_inline, hot)) verbose getSeparateValues(char* singleData) {
    verbose verboseOfProject;
    int dataLength = strlen(singleData);
    int lastIndex = 0;
    #pragma GCC unroll 4
    for (register int i = 0; i < dataLength; i++) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&singleData[i + 64], 0, 3);
        }
        if (__builtin_expect(singleData[i] == '^', 0)) {
            verboseOfProject.name = getValue(singleData, 0, i - 1);
            lastIndex = i + 1;
        } else if (__builtin_expect(singleData[i] == '#', 0)) {
            verboseOfProject.githubRepo = getValue(singleData, lastIndex, i - 1);
            lastIndex = i + 1;
        } else if (__builtin_expect(singleData[i] == '\n', 0)) {
            verboseOfProject.pid = atoi(getValue(singleData, lastIndex, i - 1));
        }
    }
}


static inline __attribute__((always_inline, hot)) void freeVerbose(verbose* verboses) {
    free(verboses->githubRepo);
    free(verboses->name);
}



static inline __attribute__((always_inline, hot)) int deleteFromPcFolder(char* ServiceName) {
    char command[100];
    size_t size = snprintf(NULL, 0, "/var/lib/%s", ServiceName);
    if (__builtin_expect(size <= 0, 0)) {
        perror("can not allocate memory for command to delete service folder on /var//lib/...\n");
        return -1;
    }
    snprintf(command, size + 1, "/var/lib/%s", ServiceName);
    if (__builtin_expect(!folderExists(command), 0)) {
        return 0;
    }
    if (__builtin_expect(deleteFolderRecursievly(command) != 0, 0)) {
        perror("deleted service directory failed!\n");
        fprintf(stderr, "try to delete %s\n", command);
        return -1;
    }
    DEBUG
    return 0;
}

static inline __attribute__((always_inline)) int killProcess(pid_t pid) {
    if (__builtin_expect(pid == 0, 0)) return 0;
    if (__builtin_expect(kill(pid, 0) != 0, 0)) { 
        return 0;
    }
    if (__builtin_expect(kill(pid, SIGKILL) != 0, 0)) {
        fprintf(stderr, "\033[31mFailed to kill the process with id of \033[33m%i\033[31mm!\033[0m\n", pid);
        return -1;
    }
    return 0;
}

OPT(hot) int normalDeleteServices(service*** __restrict__ services, char* __restrict__ serviceName) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        printf("\003[31mno services avialable\n");
        return -1;
    }
    DEBUG
    enum {FIND_SERVICE, FREE_SERVICES, FMT_SERVICES, DEL_SERVICE} state = FIND_SERVICE;
    int lastIndex = 0;
    service* foundService = NULL;
    verbose serviceVerbose;
    for (register int x = 0; x < numberOfProjects; x++) {
        if (__builtin_expect((x & 511) == 0 || x == 0, 0)) {
            __builtin_prefetch(&(*services)[x + 512], 0, 3);
        }
        if (state == FIND_SERVICE) {
            if (__builtin_expect(strcmp((*services)[x]->name, serviceName) == 0, 0)) {
                lastIndex = x;
                foundService = (*services)[x];
                if (__builtin_expect(deleteFromPcFolder(foundService->name) != 0, 0)) {
                    printf("\033[31mcan not delete your service from /var/lib/%s\033[0m\n", foundService->name);
                    printf("\033[33mmay be re run it with sudo!\033[0m\n");
                    return -1;
                }
                serviceVerbose.pid = 0;
                state = FREE_SERVICES;
            }
        }
        if (state == FREE_SERVICES) {
            service* curSer = (*services)[x];
            serviceVerbose.githubRepo = strdup(curSer->githubRepo);
            serviceVerbose.name = strdup(curSer->name);
            serviceVerbose.pid = curSer->pid;
            free(curSer->githubRepo);
            free(curSer->name);
            free(curSer);
            state = FMT_SERVICES;
        }
        if (state == FMT_SERVICES) {
            
            //(*services)[numberOfProjects - 1] = NULL;
            if (x == numberOfProjects - 1) {
                lastIndex = x;
                state = DEL_SERVICE;
            } else {
                (*services)[x] = (*services)[x + 1];
            }
            
        }
        if (state == DEL_SERVICE) {
            numberOfProjects--;
            DEBUG
            if (__builtin_expect(numberOfProjects == 0, 0)) {
                free(*services);
                *services = NULL;
                capacityOfServices = __INITIAL_SCALE_SIZE_OF_SERVICES__;
                if(__builtin_expect(killProcess(serviceVerbose.pid) == 0, 1)) {
                    serviceVerbose.pid = 0;
                }
                printf("\033[33mSuccessfully deleted service!\n\t\033[34m|-\033[35mname: \033[32m%s\n\t\033[34m|-\033[35mgithub-repo: \033[32m%s\n\t\033[34m|-\033[35mrunningpid \033[32m%i\033[0m\n", serviceVerbose.name, serviceVerbose.githubRepo, serviceVerbose.pid);
                printf("\n\tfriendly reminder: if the deleted service have pid greater than 0, just run \t\033[32mmrn stop <pid>\033[0m\n");
                return 0;
            }
            DEBUG
            if(__builtin_expect(killProcess(serviceVerbose.pid) == 0, 1)) {
                serviceVerbose.pid = 0;
            }
            printf("\033[33mSuccessfully deleted service!\n\t\033[34m|-\033[35mname: \033[32m%s\n\t\033[34m|-\033[35mgithub-repo: \033[32m%s\n\t\033[34m|-\033[35mrunningpid \033[32m%i\033[0m\n", serviceVerbose.name, serviceVerbose.githubRepo, serviceVerbose.pid);
            printf("\n\tfriendly reminder: if the deleted service have pid greater than 0, just run \t\033[32mmrn stop <pid>\033[0m\n");
            freeVerbose(&serviceVerbose);
            return 0;
        }
    }
    if (state == FIND_SERVICE) {
        printf("can not find any service with name of that!\n");
        return -1;
    }
    return -1;
}



//unused function
int delete_service_force(service*** __restrict__ services, char* __restrict__ serviceName) {
    if (services == NULL || *services == NULL) {
        printf("\003[34m may be you missed some services pointer!\n");
        goto deleteFromDisk;
    }
    for (register int x = 0; x < numberOfProjects; x++) {
        if (strcmp((*services)[x]->name, serviceName) == 0) {
            free((*services)[x]->githubRepo);
            free((*services)[x]->name);
            free((*services)[x]);
            for (register int y = x; y < numberOfProjects - 1; y++) {
                (*services)[y] = (*services)[y + 1];
            }
            numberOfProjects--;
            service** tmp = realloc(services, (capacityOfServices - 1) * sizeof(service*));
            if (tmp == NULL) {
                perror("can not reallocate memory for deletion of services!\n");
                exit_program(-1)
            }
            if (numberOfProjects == 0) {
                free((*services));
                *services = NULL;
                goto deleteFromDisk;
            }
            *services = tmp;
            goto deleteFromDisk;

        }
    }
    deleteFromDisk:
        int projectsFileFd = open("data/projects", O_CREAT | O_RDWR, 0644);
        if (projectsFileFd == -1) {
            perror("failed to open projects file!\n");
            exit_program(-1)
        }
        struct stat st;
        if (fstat(projectsFileFd, &st) != 0) {
            perror("fstat failed on projects file!\n");
            exit_program(-1)
        }
        if (ftruncate(projectsFileFd, st.st_size) != 0) {
            perror("ftruncate failed on projects file!\n");
            exit_program(-1)
        }
        char* data = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, projectsFileFd, 0);
        if (data == MAP_FAILED) {
            perror("mmap failed on projects file!\n");
            exit_program(-1)
        }
        int lastPindex = 0;
        int lastHindex = 0;
        _Bool first = 0;
        for (register int i = 0; i < st.st_size; i++) {
            if (!first) {
                if (data[i] == '^') {
                    char* name = getValue(data, 0, i - 1);
                    if (name == NULL) {
                        perror("can not get service name exiting...\n");
                        exit_program(-1)
                    }
                    if (strcmp(name, serviceName) == 0) {
                        for (register int j = i + 1; j < st.st_size; j++) {
                            if (data[j] == '\n') {
                                char* newServices = cutString(data, 0, j);
                                char* removedServiceString = getValue(data, 0, j);
                                verbose serviceVerbose = getSeparateValues(removedServiceString);
                                free(removedServiceString);
                                if (newServices == NULL) {
                                    fprintf(stderr, "can not delete service memory allocation failed for newServices!... %s: %i\n", __FILE__, __LINE__);
                                    exit_program(-1)
                                }
                                strcpy(data, newServices);
                                msync(data, st.st_size, MS_ASYNC);
                                munmap(data, st.st_size);
                                close(projectsFileFd);
                                free(name);
                                free(newServices);
                                printf("\033[33mSuccessfully deleted service!\n\t\033[34m|-\033[35mname: \033[32m%s\n\t\033[34m|-\033[35mgithub-repo: \033[32m%s\n\t\033[34m|-\033[35mrunningpid \033[32m%i\033[0m\n", serviceVerbose.name, serviceVerbose.githubRepo, serviceVerbose.pid);
                                printf("\n\tfriendly reminder: if the deleted service have pid greater than 0, just run \t\033[32mmrn stop <pid>\033[0m\n");
                                return 0;
                            }
                        }
                    }
                    free(name);
                } else if (data[i] == '\n') {
                    lastPindex = i + 1;
                    first = 1;
                }
            } else {
                if (data[i] == '^') {
                    char* name = getValue(data, lastPindex, i - 1);
                    if (name == NULL) {
                        perror("can not get name of the service!\n");
                        exit_program(-1)
                    }
                    if (strcmp(name, serviceName) == 0) {
                        for (register int j = i+1; i < st.st_size; j++) {
                            if (data[j] == '\n') {
                                char* newServices = cutString(data, 0, j);
                                char* removedServiceString = getValue(data, 0, j);
                                verbose serviceVerbose = getSeparateValues(removedServiceString);
                                free(removedServiceString);
                                if (newServices == NULL) {
                                    fprintf(stderr, "failed to allocte memory on newServices string! ... %s: %i\n", __FILE__, __LINE__);
                                    exit_program(-1)
                                }
                                strcpy(data, newServices);
                                msync(data, st.st_size, MS_ASYNC);
                                munmap(data, st.st_size);
                                close(projectsFileFd);
                                free(name);
                                free(newServices);
                                printf("\033[33mSuccessfully deleted service!\n\t\033[34m|-\033[35mname: \033[32m%s\n\t\033[34m|-\033[35mgithub-repo: \033[32m%s\n\t\033[34m|-\033[35mrunningpid \033[32m%i\033[0m\n", serviceVerbose.name, serviceVerbose.githubRepo, serviceVerbose.pid);
                                printf("\n\tfriendly reminder: if the deleted service have pid greater than 0, just run \t\033[32mmrn stop <pid>\033[0m\n");
                                return 0;
                            }
                        }
                    }
                    free(name);
                }
            }
        }
        printf("can not find any project with a name:\t\033[32m-\033[32m%s\033[0m\n", serviceName);
        return -1;
}