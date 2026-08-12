#pragma optimize("O3")
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
//#define DEBUG_MODE 1
[[nodiscard]]__attribute__((hot)) int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName, pid_t pid) {
    if (__builtin_expect(services == NULL, 0)) {
        service** tmp = malloc(__INITIAL_SCALE_SIZE_OF_SERVICES__ * sizeof(service*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("can not allocate memory for services!\n");
            exit_program(-1)
        }
        (*services) = tmp;
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 64], 0, 3);
        }
        if (__builtin_expect(strcmp(githubRepo, (*services)[i]->githubRepo) == 0, 0)) {
            printf("\033[33m\tgithub repo already exists!\033[0m\n");
            return -1;
        }
        if (__builtin_expect(strcmp(nickName, (*services)[i]->name) == 0, 0)) {
            fprintf(stderr, "\033[31m|-\033[33m%s \033[0malready exists for another servies of %s\ncode panick!\033[0m\n", nickName, (*services)[i]->githubRepo);
            return -1;
        }
    }
    service* newService = malloc(sizeof(service));
    if (__builtin_expect(newService == NULL, 0)) {
        printf("memory allocation for new service failed!\n");
        return -1;
    }
    newService->cloned = 1;
    newService->githubRepo = strdup(githubRepo);
    newService->name = strdup(nickName);
    newService->pid = pid;
    if (__builtin_expect(capacityOfServices <= numberOfProjects, 0)) {
        service** tmp = realloc(*services, (capacityOfServices + __INITIAL_SCALE_SIZE_OF_SERVICES__) * (sizeof(service*)));
        if (__builtin_expect(tmp == NULL, 0)) {
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
    if (__builtin_expect((*services)[numberOfProjects]->githubRepo == NULL, 0)) {
        printf("\033[31mcan not add project!\033[0m\n");
        return -1;
    }
    numberOfProjects++;
    #ifdef DEBUG_MODE
        printf("loaded\n\tname: %s\n\tgithubRepo: %s\n\t cloned: %i\n\tpid: %i\n", newService->name, newService->githubRepo, newService->cloned, newService->pid);
    #endif
    return 0;
}