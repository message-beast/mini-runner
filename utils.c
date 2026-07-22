#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "base/structure.h"
#include "base/config.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "basic.h"
#include <sys/wait.h>
#include <threads.h>

typedef struct param {
    char* command;
    service* service;
} param;

int cloneRepo(service** services);
void freeServices(service*** services);


[[nodiscard]]int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName) {
    if (services == NULL) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (tmp == NULL) {
            perror("can not allocate memory for services!\n");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (strcmp(githubRepo, (*services)[i]->githubRepo) == 0) {
            printf("github repo already exists!\n");
            return -1;
        }
        if (strcmp(nickName, (*services)[i]->name) == 0) {
            fprintf(stderr, "name already exists for the another servies of %s\n", (*services)[i]->githubRepo);
            return -1;
        }
    }
    printf("numberofProjects:%i\n", numberOfProjects);
    service* newService = calloc(1, sizeof(service));
    if (newService == NULL) {
        printf("memory allocation for new service failed!\n");
        return -1;
    }
    newService->cloned = 1;
    newService->githubRepo = githubRepo;
    newService->name = nickName;
    newService->pid = 0;
    if (capacityOfServices <= numberOfProjects) {
        service** tmp = realloc(*services, capacityOfServices + __INITIAL_SCALE_SIZE_OF_SERVICES__);
        if (tmp == NULL) {
            perror("can not resize memory for additional of services");
            exit_program(-1)
        }
        (*services) = tmp;
    }

    (*services)[numberOfProjects] = newService;
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );
    if ((*services)[numberOfProjects]->githubRepo == NULL || (*services)[numberOfProjects]->pid != 0) {
        printf("can not add project!\n");
        return -1;
    }
    numberOfProjects++;
    printf("loaded\n\tname: %s\n\tgithubRepo: %s\n\t cloned: %i\n\tpid: %i\n", newService->name, newService->githubRepo, newService->cloned, newService->pid);
    return 0;
}


[[nodiscard]]int addProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName) {
    if (services == NULL) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (tmp == NULL) {
            perror("can not allocate memory for services!\n");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (strcmp(githubRepo, (*services)[i]->githubRepo) == 0) {
            printf("github repo already exists!\n");
            return -1;
        }
        if (strcmp(nickName, (*services)[i]->name) == 0) {
            fprintf(stderr, "name already exists for the another servies of %s\n", (*services)[i]->githubRepo);
            return -1;
        }
    }
    printf("numberofProjects:%i\n", numberOfProjects);
    service* newService = calloc(1, sizeof(service));
    if (newService == NULL) {
        printf("memory allocation for new service failed!\n");
        return -1;
    }
    newService->cloned = 0;
    newService->githubRepo = githubRepo;
    newService->name = nickName;
    newService->pid = 0;
    if (capacityOfServices <= numberOfProjects) {
        service** tmp = realloc(*services, (capacityOfServices + __INITIAL_SCALE_SIZE_OF_SERVICES__) * sizeof(service*));
        if (tmp == NULL) {
            perror("can not resize memory for additional of services");
            exit_program(-1)
        }
        (*services) = tmp;
        capacityOfServices += __INITIAL_SCALE_SIZE_OF_SERVICES__;
    }
    (*services)[numberOfProjects] = newService;
    __asm__ volatile (
        "mfence"
        :
        :
        : "memory"
    );
    if ((*services)[numberOfProjects]->githubRepo == NULL || (*services)[numberOfProjects]->pid != 0) {
        printf("can not add project!\n");
        return -1;
    }
    numberOfProjects++;
    printf("added\n\tname: %s\n\tgithubRepo: %s\n\t cloned: %i\n\tpid: %i\n", newService->name, newService->githubRepo, newService->cloned, newService->pid);
    if (cloneRepo((*services)) != 0) {
        perror("can not clone the repository!\n");
        freeServices(services);
        return -1;
    }
    return 0;
}

int cloneRepo(service** services) {
    if (services == NULL) {
        perror("services is empty!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (services[i]->cloned == 0) {
            size_t size = snprintf(NULL, 0, "sudo git clone %s /var/lib/%s", services[i]->githubRepo, services[i]->name);
            if (size <= 0) {
                exit_program(-1)
            }
            char* command = calloc(size, sizeof(char));
            snprintf(command, size + 1, "sudo git clone %s /var/lib/%s", services[i]->githubRepo, services[i]->name);
            if (command == NULL) {
                exit_program(-1)
            }
            int pid = fork();
            if (pid == 0) {
                printf("command to run: %s\n", command);
                system(command);
                free(command);
                exit(0);
            } else {
                int status;
                waitpid(pid, &status, 0);
                free(command);
                if (WEXITSTATUS(status) != 0) {
                    perror("\n\033[31mcan not clone repository!\033[0m\n");
                    exit_program(1)
                }
                services[i]->cloned = 1;
            }
        }
    }
    return 0;

}


int runthread(void* paramss) {
    param* params = (param*)paramss;
    int pid = fork();
    if (pid == 0) {
        params->service->pid = getpid();
        execlp(params->command, params->command, NULL);
        perror("can not run service!\n");
        exit_program(1)
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != 0) {
            perror("can not run service!\n");
            exit_program(1)
        }
    }
    free(params);
    params = NULL;
    return 0;
}



int runService(service** __restrict__ services, char* __restrict__ name, char* __restrict__ bash) {
    
    if (bash == NULL) {
        bash = "run.sh";
    }
    //read memory address that anything is not on update
    for (register int i = 0; i < numberOfProjects; i++) {
        if (strcmp(services[i]->name, name)) {
            size_t size = snprintf(NULL, 0, "bash /var/lib/%s", bash);
            if (size <= 0) {
                perror("size can not be dynamically calculate!\n");
                exit_program(-1)
            }
            char* command = calloc(size, sizeof(char));
            if (command == NULL) {
                perror("can not allocate memory for command");
                exit_program(-1)
            }
            snprintf(command, size, "bash var/lib/%s", bash);
            thrd_t process_thread;
            param* params = calloc(1, sizeof(param));
            params->command = command;
            params->service = services[i];
            thrd_create(&process_thread, runthread, (void*)params);
            thrd_detach(process_thread);
        }
    }
}


[[nodiscard]] char* getUpdateStatus() {
    int updateStatusSharedFileFd = open("data/updateStatus", O_RDWR, 0644, NULL);
    if (updateStatusSharedFileFd == -1) {
        perror("failed to open shared update status file to read!\n");
        return NULL;
    }

    struct stat st;
    if (fstat(updateStatusSharedFileFd, &st) != 0) {
        perror("fstat failed!");
        return NULL;
    }
    char* buffer;
    read(updateStatusSharedFileFd, buffer, 15);
    return (buffer) ? buffer : NULL;
}


_Bool fileExists(char* filePath) {
    struct stat st;
    return (stat(filePath, &st) == 0);
}


void freeServices(service*** services) {
    if (services == NULL || *services == NULL) {
        return;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if ((*services)[i] != NULL) {
            free((*services)[i]->githubRepo);
            free((*services)[i]->name);
            free((*services)[i]);
        }
    }
    if ((*services) != NULL) {
        free(*services);
    }
    *services = NULL;
    numberOfProjects = 0;
}