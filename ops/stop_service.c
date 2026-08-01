#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../basic.h"
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
static inline __attribute__((always_inline)) int killProcess(pid_t pid) {
    if (kill(pid, SIGTERM) != 0) {
        fprintf(stderr, "\033[31mFailed to kill the process with id of \033[33m%i\033[31mm!\033[0m\n", pid);
        return -1;
    }
    printf("\033[32mprocess killed by its id \033[33m%i\033[0m\n", pid);
    return 0;
}


int stopService(service*** __restrict__ services, char* __restrict__ serviceName) {
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (strcmp((*services)[i]->name, serviceName) == 0) {
            DEBUG
            service* currentService = (*services)[i];
            printf("its pid is %i\n", currentService->pid);
            printf("service to stop:\n\t\033[31m|\033[33m-\033[34mname: \033[32m%s\n\t\033[31m|\033[33m-\033[34mgithubRepo: \033[32m%s\n\t\033[31m|\033[33m-\033[34mpid: \033[32m%i\n", currentService->name, currentService->githubRepo, currentService->pid);
            if (currentService->pid == 0) {
                return 0;
            }
            if (killProcess(currentService->pid) != 0) {
                return -1;
            }
            currentService->pid = 0;
            return 0;
        }
    }
    fprintf(stderr, "\033[31mcan not find any service with name \033[33m%s\033[0m\n", serviceName);
    return -1;
}