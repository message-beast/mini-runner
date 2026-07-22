#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "init/main.h"
#include "basic.h"
#include "ops/create_service.h"
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
    save_services(services);
    freeServices(&services);
}


int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("\033[35m Welcome to mrn (Mini-Runner) built for vps users to automate thigs and then to reduce cloud costs,\n\n\033[33m Developed by Melikt Belay!\n\n\033[0mHere is major uses cases you can make:\n\n\tmrn add <github repo> -- this adds the github repo to your process and then update them when the update is going!\n\tmrn stop -- this stops all running processes of your miroservices and macroservices\n\tmrn run <bash to run> --this just runs your bash on linux vps to start some services or servers\n\tmrn update --this updates and pull github requests if it fails you just do the merge your self\n\n\tthanks for using mrn it is well powered c program that just needs tiny memory and cpu to do the work\n");
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
                freeServices(&services);
                return 1;
            }
            continue;
        }


    }
    return 0;
}