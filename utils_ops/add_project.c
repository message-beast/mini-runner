#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../basic.h"
#include <sys/wait.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
int cloneRepo(service** services);
[[nodiscard]]int addProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName) {
    if (__builtin_expect(services == NULL, 0)) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (tmp == NULL) {
            perror("can not allocate memory for services!\n");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect(strcmp(githubRepo, (*services)[i]->githubRepo) == 0, 0)) {
            printf("\033[33m\tgithub repo already exists!\033[0m\n");
            return -1;
        }
        if (__builtin_expect(strcmp(nickName, (*services)[i]->name) == 0, 0)) {
            fprintf(stderr, "\033[31m|-\033[33m%s \033[0malready exists for another servies of %s\033[0m\n", nickName, (*services)[i]->githubRepo);
            return -1;
        }
    }
    service* newService = malloc(sizeof(service));
    if (__builtin_expect(newService == NULL, 0)) {
        printf("memory allocation for new service failed!\n");
        return -1;
    }
    newService->cloned = 0;
    newService->githubRepo = strdup(githubRepo);
    newService->name = strdup(nickName);
    newService->pid = 0;
    if (__builtin_expect(capacityOfServices <= numberOfProjects, 0)) {
        service** tmp = realloc(*services, (capacityOfServices + __INITIAL_SCALE_SIZE_OF_SERVICES__) * sizeof(service*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("can not resize memory for additional of services");
            free(newService);
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
    if (__builtin_expect((*services)[numberOfProjects]->githubRepo == NULL || (*services)[numberOfProjects]->pid != 0, 0)) {
        printf("\033[31mcan not add project!\033[0m\n");
        free(newService);
        return -1;
    }
    numberOfProjects++;
    printf("Service info to add\n\t\033[33m|\033[31m-\033[33mname: \033[32m%s\n\t\033[33m|\033[31m-\033[33mgithubRepo: \033[32m%s\n\t\033[33m|\033[31m-\033[33m cloned: \033[32m%i\n\t\033[33m|\033[31m-\033[33mpid: \033[32m%i\033[0m\n", newService->name, newService->githubRepo, newService->cloned, newService->pid);
    if (__builtin_expect(cloneRepo((*services)) != 0, 0)) {
        fprintf(stderr, "\033[31mcan not clone the repository!\033[0m\n");
        return -1;
    }
    return 0;
}

int cloneRepo(service** services) {
    if (__builtin_expect(services == NULL, 0)) {
        perror("services is empty!\n");
        return -1;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect(services[i]->cloned == 0, 1)) {
            size_t size = snprintf(NULL, 0, "sudo git clone %s /var/lib/%s", services[i]->githubRepo, services[i]->name);
            if (size <= 0) {
                exit_program(-1)
            }
            char* command = calloc(size + 1, sizeof(char));
            snprintf(command, size + 1, "sudo git clone %s /var/lib/%s", services[i]->githubRepo, services[i]->name);
            if (__builtin_expect(command == NULL, 0)) {
                exit_program(-1)
            }
            int pipeFd[2];
            pipe(pipeFd);
            int pid = fork();
            if (pid == 0) {
                close(pipeFd[0]);
                printf("command to run: %s\n", command);
                if(system(command) != 0) {
                    write(pipeFd[1], "failed", 7);
                } else {
                    write(pipeFd[1], "success", 8);    
                }
                close(pipeFd[1]);
                free(command);
                abort();
            } else {
                close(pipeFd[1]);
                int status;
                waitpid(pid, &status, 0);
                free(command);
                #if defined(DEBUG_MODE)
                    printf("cloning exit status: %i\n", WEXITSTATUS(status));
                #endif
                if (__builtin_expect(WEXITSTATUS(status) != 0, 0)) {
                    perror("\n\033[31mcan not clone repository!\033[0m\n");
                    exit_program(-1)
                }
                char buff[10];
                read(pipeFd[0], buff, 10);
                close(pipeFd[0]);
                if (__builtin_expect(strcmp(buff, "failed") == 0, 0)) {
                    return -1;
                }
                services[i]->cloned = 1;
            }
        }
    }
    return 0;

}