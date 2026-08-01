#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "init/main.h"
#include "basic.h"
#include "ops/create_service.h"
#include "ops/delete_service.h"
#include "ops/stop_service.h"
#include "ops/show_services.h"
#define true 1
#define false 0
struct service** services = NULL;






__attribute__((constructor))
void init() {
    if (!fileExists("data/updateStatus")) {
        if (initiateMemoryPtr() == 0) {
            exit_program(-1)
        }
    }
    if (services == NULL) {
        service** tmp = calloc(__INITIAL_SCALE_SIZE_OF_SERVICES__, sizeof(service*));
        if (tmp == NULL) {
            perror("memory allocation for services failed!\n");
            exit_program(-1)
        }
        services = tmp;
    }
    loadServices(&services);
}




__attribute__((destructor))
void closeProcess() {
    #if defined(DEBUG_MODE)
        printf("destructor called!\n");
    #endif
    if (save_services(&services) != 0) {
        printf("save_service is not healthy!");
    }
    freeServices(&services);
}

static inline void displayHelp() {
    printf("\033[32m Welcome to mrn (Mini-Runner) built for vps users to automate thigs and then to reduce cloud costs,\n\n\033[33m Developed by Melikt Belay!\n\n\033[0mHere is major uses cases you can make:\n\n\tmrn add <github repo> -- this adds the github repo to your process and then update them when the update is going!\n\tmrn stop -- this stops all running processes of your miroservices and macroservices\n\tmrn run <bash to run> --this just runs your bash on linux vps to start some services or servers\n\tmrn update --this updates and pull github requests if it fails you just do the merge your self\n\n\tthanks for using mrn it is well powered c program that just needs tiny memory and cpu to do the work\n");
}


int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            displayHelp();
            return 0;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max") == 0) {
            int numberofProcesses = atoi(argv[i+1]);
            services = realloc(services, numberofProcesses * sizeof(service*));
            if (services == NULL) {
                perror("memory allocation for services failed!\n");
                exit_program(-1)
            }
            continue;
        } else if (strcmp(argv[i], "add") == 0) {
            char* githubLink = argv[i + 1];
            char* nickName = argv[i + 2];
            if(addProject(&services, githubLink, nickName) != 0) {
                return 1;
            }
            continue;
        } else if (strcmp(argv[i], "remove") == 0) {

            char* serviceName = argv[i + 1];
            if (normalDeleteServices(&services, serviceName) != 0) {
                fprintf(stderr, "\033[31mcan't delete %s\033[0m\n]", serviceName);
            }
        } else if (strcmp(argv[i], "run") == 0) {
            char* name = argv[i + 1];
            char* bash = argv[i + 2];
            if (strlen(name) <= 0 || name == NULL) {
                fprintf(stderr, "invalid name argument\n");
            }
            _Bool attach = false;
            for (register int j = i + 1; j < argc; j++) {
                if(strcmp(argv[j], "--attach") == 0) {
                    attach = true;
                    break;
                }
            }
            if (runService(&services, name, bash, attach) != 0) {
                fprintf(stderr, "can not run service \"%s\"\n", name);
            }
            printf("\033[32mservice \"\033[33m%s\033[0m\"\033[32m running successfully...\033[0m\n", name);
            
        } else if (strcmp(argv[i], "stop") == 0) {
            for (register int j = i + 1; j < argc; ++j) {
                if (stopService(&services, argv[j]) != 0) {
                    printf("can not kill the service with name %s\n", argv[j]);
                }
            }
        } else if (strcmp(argv[i], "list") == 0) {
            listServices(&services);
        } else if (strcmp(argv[i], "find") == 0) {
            DEBUG
            char* searchName = argv[i + 1];
            for (register int j = i + 2; j < argc; j++) {
                if (strcmp(argv[j], "--sort") == 0) {
                    DEBUG
                    search(&services, searchName, 1);
                    return 0;
                }
            }
            if (searchName == NULL) {
                printf("service name not provideed for search!");
                return 1;
            }
            DEBUG
            search(&services, searchName, 0);
        }


    }
    return 0;
}