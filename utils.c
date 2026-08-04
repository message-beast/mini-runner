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

int cloneRepo(service** services);
void freeServices();
_Atomic _Bool pidSet = 0;
pthread_mutex_t mutex;

[[nodiscard]]int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName, pid_t pid) {
    if (services == NULL) {
        service** tmp = malloc(__INITIAL_SCALE_SIZE_OF_SERVICES__ * sizeof(service*));
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
    #if defined(DEBUG_MODE)
        printf("loaded\n\tname: %s\n\tgithubRepo: %s\n\t cloned: %i\n\tpid: %i\n", newService->name, newService->githubRepo, newService->cloned, newService->pid);
    #endif
    return 0;
}


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


static inline __attribute__((always_inline)) int writePid(service** __restrict__ services, char* __restrict__ serviceName, int pid) {
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        fprintf(stderr, "can not run %s\n becuse services are empty!\n", serviceName);
        return -1;
    }
    (*services)->pid = pid;
    return 0;
}


void* runthread(void* paramss) {
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


int runService(service*** __restrict__ services, char* __restrict__ name, char* __restrict__ bash, _Bool attach) {
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
    for (register int i = 0; i < numberOfProjects; i++) {
        if (strcmp((*services)[i]->name, name) == 0) {
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
                pthread_detach(process_thread);
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


int warmService(service** __restrict__ services, char* __restrict__ bash, _Bool attach) {
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
        pthread_detach(process_thread);
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






/*[[nodiscard]] char* getUpdateStatus() {
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
}*/


_Bool fileExists(char* filePath) {
    struct stat st;
    return (stat(filePath, &st) == 0);
}


void freeServices(service*** __restrict__ services) {
    if (__builtin_expect(services == NULL || (*services) == NULL, 0)) {
        return;
    }
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((*services)[i] != NULL, 1)) {
            free((*services)[i]->githubRepo);
            free((*services)[i]->name);
            free((*services)[i]);
        }
    }
    if (__builtin_expect(*services != NULL, 1)) {
        free(*services);
    }
    *services = NULL;
    numberOfProjects = 0;
    capacityOfServices = 0;
}