#pragma optimize("03")
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
#define true 1
#define false 0
typedef struct param {
    service** service;
    char* command;
    char* name;
    _Bool attach;
} param;
_Atomic _Bool pidSet = 0;
pthread_mutex_t mutex;

static inline __attribute__((always_inline, hot)) int writePid(service** __restrict__ services, char* __restrict__ serviceName, int pid) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        fprintf(stderr, "can not run %s\n becuse services are empty!\n", serviceName);
        return -1;
    }
    (*services)->pid = pid;
    return 0;
}


static inline __attribute__((always_inline, hot)) void* runthread(void* paramss) {
    param* params = (param*)paramss;
    service* ser = *(params->service);
    size_t size = snprintf(NULL, 0, "/var/lib/%s", ser->name);
    if (__builtin_expect(size <= 0, 0)) {
        perror("can not store command to change directory!\n");
        atomic_exchange(&pidSet, 1);
        return NULL;
    }
    char path[size + 1];
    snprintf(path, size + 1, "/var/lib/%s", ser->name);
    if (__builtin_expect(path == NULL, 0)) {
        perror("can not create command to navigate directory!\n");
        atomic_exchange(&pidSet, 1);
        return NULL;
    }
    printf("passed\n");
    int pipeFd[2];
    pipe(pipeFd);
    int pid = fork();
    if (pid == 0) {
        close(pipeFd[0]);
        if (chdir(path) != 0) {
            perror("failed to change directory!\n");
            write(pipeFd[1], "0", 2);
            close(pipeFd[1]);
            atomic_exchange(&pidSet, 1);
            return NULL;
        }
        char response[10];
        snprintf(response, 10, "%i", getpid());
        write(pipeFd[1], response, 10);
        close(pipeFd[1]);
        DEBUG
        __asm__ volatile (
            "sfence" 
            :
            :
            : "memory"
        );
        printf("thread running with pid: %i\n", getpid());
        printf("command: %s\n", params->command);
        if (params->attach == false) {
            DEBUG
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            close(STDIN_FILENO);
        } 
        execlp("bash", "bash", params->command, NULL);
        free(params->command);
        printf("child failed!\n");
        close(pipeFd[0]);
        snprintf(response, 10, "%i", 0);
        write(pipeFd[1], response, 10);
        close(pipeFd[1]);
        free(params);
        abort();
    } else {
        DEBUG
        __asm__ volatile ("sfence" ::: "memory");
        close(pipeFd[1]);
        char buff[10];
        read(pipeFd[0], buff, 10);
        int newPid = atoi(buff);
        if (__builtin_expect(newPid != 0, 1)) {
            printf("\033[32mservice \"\033[33m%s\033[0m\"\033[32m running successfully...\033[0m\n", params->name);
        }
        printf("writing pid pof %i\n", newPid);
        pthread_mutex_lock(&mutex);
        ser->pid = newPid;
        atomic_exchange(&pidSet, 1);
        pthread_mutex_unlock(&mutex);
        close(pipeFd[0]);
    }
    free(params->command);
    free(params);
    params = NULL;
    return NULL;
}


static inline __attribute__((always_inline, hot)) _Bool updateAvialable() {
    int updateStatusFileFd = open("data/updateStatus", O_CREAT | O_RDWR, 0644, NULL);
    if (__builtin_expect(updateStatusFileFd == -1, 0)) {
        perror("can not open data/updateStatus file!\n");
        return 0;
    }
    struct stat st;
    if (__builtin_expect(fstat(updateStatusFileFd, &st) != 0, 0)) {
        perror("fstat failed on updateStatus!\n");
        return 0;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, updateStatusFileFd, 0);
    if (strcmp("idle", data) == 0) {
        return 0;
    }
    return 1;
}



int volatile running = 0;


void returnServicePid(int sig) {
    running = sig;
    return;
}
static inline __attribute__((always_inline, hot)) void backup(service** foundService) {
    if (__builtin_expect(foundService == NULL || *foundService == NULL, 0)) {
        return;
    }
    (*foundService)->pid = 0;
    return;
}


__attribute__((hot)) int runService(service*** __restrict__ services, char* __restrict__ name, char* __restrict__ bash, _Bool attach) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        fprintf(stderr, "\033[31mcan not run service \033[33m%s\033[0m, services empty!\n", name);
        return -1;
    }
    if (__builtin_expect(name == NULL || strlen(name) <= 0, 0)) {
        fprintf(stderr, "invalid service name!\n");
        return -1;
    }
    if (bash == NULL) {
        bash = "run.sh";
    }
    if (updateAvialable()) {
        printf("\033[33mplease stay tuned! your services are being updated right now!\033[0m\n");
        return -1;
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, name) == 0, 0)) {
            size_t size = snprintf(NULL, 0, "/var/lib/%s/%s", name, bash);
            if (__builtin_expect(size <= 0, 0)) {
                perror("size can not be dynamically calculate!\n");
                exit_program(-1)
            }
            char* command = malloc(size + 1);
            if (__builtin_expect(command == NULL, 0)) {
                perror("can not allocate memory for command");
                exit_program(-1)
            }
            snprintf(command, size + 1, "/var/lib/%s/%s", name, bash);
            pthread_t process_thread;
            pthread_mutex_init(&mutex, NULL);
            param* params = malloc(sizeof(param));
            params->command = strdup(command);
            params->service = &((*services)[i]);
            params->name = name;
            params->attach = attach;
            printf("setting pointer of %p\n", ((*services)[i]));
            __asm__ volatile (
                "sfence"
                :
                :
                : "memory"
            );
            if (__builtin_expect(pthread_create(&process_thread, NULL, runthread, (void*)params) != 0, 0)) {
                perror("can not run service! failed to create thread!\n");
                free(params);
                free(command);
                return -1;
            }
            if (!attach) {
                if (__builtin_expect(pthread_detach(process_thread) != 0, 0)) {
                    perror("Failed to create a detached pthread!\n");
                    return -1;
                }
            } else {
                signal(SIGINT, returnServicePid);
                while (!running) {
                    pause();
                }
                if (running == SIGINT) {
                    backup(&((*services)[i]));
                }
            }
            while (!atomic_load(&pidSet));
            printf("it spid is %i\n", (*services)[i]->pid);
            free(command);
            return 0;
        }
    }
    fprintf(stderr, "\033[31mcan not find any service with name \033[33m%s\033[0m\n!", name);
    return -1;
}


__attribute__((hot)) int warmService(service** __restrict__ services, char* __restrict__ bash, _Bool attach) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        fprintf(stderr, "\033[31mcan not run service \033[33m%s\033[0m, services empty!\n", (*services)->name);
        return -1;
    }
    char* name = (*services)->name;
    if (__builtin_expect(name == NULL || strlen(name) <= 0, 0)) {
        fprintf(stderr, "invalid service name!\n");
        return -1;
    }
    if (bash == NULL) {
        bash = "run.sh";
    }
    if (updateAvialable()) {
        printf("\033[33mplease stay tuned! your services are being updated right now!\033[0m\n");
        return -1;
    }
    size_t size = snprintf(NULL, 0, "/var/lib/%s/%s", name, bash);
    if (__builtin_expect(size <= 0, 0)) {
        perror("size can not be dynamically calculate!\n");
        exit_program(-1)
    }
    char* command = malloc(size + 1);
    if (__builtin_expect(command == NULL, 0)) {
        perror("can not allocate memory for command");
        exit_program(-1)
    }
    snprintf(command, size + 1, "/var/lib/%s/%s", name, bash);
    pthread_t process_thread;
    pthread_mutex_init(&mutex, NULL);
    param* params = malloc(sizeof(param));
    params->command = strdup(command);
    params->service = services;
    params->name = name;
    params->attach = attach;
    __asm__ volatile (
        "sfence"
        :
        :
        : "memory"
    );
    if (__builtin_expect(pthread_create(&process_thread, NULL, runthread, (void*)params) != 0, 0)) {
        perror("can not run service! failed to create thread!\n");
        free(params);
        free(command);
        return -1;
    }
    if (!attach) {
        if(__builtin_expect(pthread_detach(process_thread) != 0, 0)) {
            perror("Failed to create a detached pthread!\n");
            return -1;
        }
    } else {
        signal(SIGINT, returnServicePid);
        while (!running) {
            pause();
        }
        if (running == SIGINT) {
            backup(services);
        }
    }
    while (!atomic_load(&pidSet));
    printf("it spid is %i\n", (*services)->pid);
    free(command);
    return 0;
}

