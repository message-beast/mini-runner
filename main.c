#pragma optimize("O3")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "init/main_init.h"
#include "basic.h"
#include "ops/create_service.h"
#include "ops/delete_service.h"
#include "ops/stop_service.h"
#include "ops/show_services.h"
#include "ops/update_service.h"
#include "ops/restart_service.h"
#include "exceptions/services/services_exceptions.h"
#include "exceptions/messages/services/help.h"
#include "ops/rename_service.h"
#include "res_man/cpu/cpu_limit.h"
#include "exceptions/resources/cpu_violations.h"
#include "base/config.h"
#include "res_format/mem_types.h"
#include "exceptions/messages/mem_type/help.h"
#include "res_man/mem/mem_limit.h"
#include "res_man/usage/show_usage.h"
#include "res_man/usage/show_limit.h"
#define true 1
#define false 0
struct service** services = NULL;





__attribute__((constructor))
void init() {
    if (!fileExists("data/updateStatus")) {
        if (initiateMemoryPtr() == 0) {
            DEBUG
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
    DEBUG
    loadServices(&services);
    DEBUG
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

static inline __attribute((always_inline)) void displayHelp() {
    printf("\033[32m Welcome to mrn (Mini-Runner/Minimal-Resource-Usage Runner) built for vps users to automate thigs and then to reduce cloud costs,\n\n\033[33m Developed by Melikt Belay!\n\n\033[0mHere is major uses cases you can make:\n\n\tmrn add <github repo> -- this adds the github repo to your process and then update them when the update is going!\n\tmrn stop -- this stops all running processes of your miroservices and macroservices\n\tmrn run <bash to run> --this just runs your bash on linux vps to start some services or servers\n\tmrn update --this updates and pull github requests if it fails you just do the merge your self\n\tmrn stop <service name> -- this stops the service by its name\n\tmrn list -- this lists all the services\n\tmrn find <service name> -- this finds service by its name pattern matching, it is developed if you incase forget the name of your service\n\t\t --sort this sorts the results depending on their name matching score\n\tmrn run <service name> -- this runs your service but you can specify the running file after it\n\t\t --example: mrn run myservice run.sh\n\t\t\t    mrn run myservice run.sh --attach (attach is if you are looking to interact your service)\n\tmrn rename <old service name> <new service name> -- this changes the name of your service also renames the service program directory\n\tmrn neglect-updates --this just ignores if the updates avialable or not  and back to normal state to run your services\n\tmrn restart <service name> -- this restarts the services that is running/not\n\t\t--example mrn restart myservice\n\t\t\t  mrn restart myservice --attach (attach is just if you wanna test it interacting with it)\n\n\tthanks for using mrn it is well powered c program that just needs tiny memory and cpu to do the work\n");
}


int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            displayHelp();
            return 0;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max") == 0) {
            int numberofProcesses = atoi(argv[i+1]);
            services = realloc(services, numberofProcesses * sizeof(service*));
            if (__builtin_expect(services == NULL, 0)) {
                perror("memory allocation for services failed!\n");
                exit_program(-1)
            }
            continue;
        } else if (strcmp(argv[i], "add") == 0) {
            char* githubLink = argv[i + 1];
            char* nickName = argv[i + 2];
            if (__builtin_expect(isValidName(nickName) != 0, 0)) {
                printf("\033[31mRule Violated!\033[0m\n");
                displayServiceRules();
                return 1;
            }
            if (__builtin_expect(isValidGithubRepo(githubLink) != 0, 0)) {
                printf("\033[31mRule Violated!\033[0m\n");
                displayServiceRules();
                return 1;
            }
            if(__builtin_expect(addProject(&services, githubLink, nickName) != 0, 0)) {
                return 1;
            }
            continue;
        } else if (strcmp(argv[i], "remove") == 0) {
            char* serviceName = argv[i + 1];
            if (__builtin_expect(normalDeleteServices(&services, serviceName) != 0, 0)) {
                fprintf(stderr, "\033[31mcan't delete %s\033[0m\n]", serviceName);
            }
        } else if (strcmp(argv[i], "run") == 0) {
            char* name = argv[i + 1];
            char* bash = argv[i + 2];
            if (__builtin_expect(name == NULL || strlen(name) <= 0, 0)) {
                fprintf(stderr, "invalid name argument\n");
                return 1;
            }
            _Bool attach = false;
            for (register int j = i + 1; j < argc; j++) {
                if(strcmp(argv[j], "--attach") == 0) {
                    attach = true;
                    break;
                }
            }
            if (__builtin_expect(runService(&services, name, bash, attach) != 0, 0)) {
                fprintf(stderr, "can not run service \"%s\"\n", name);
            }
            
        } else if (strcmp(argv[i], "stop") == 0) {
            for (register int j = i + 1; j < argc; ++j) {
                if (__builtin_expect(stopService(&services, argv[j]) != 0, 0)) {
                    printf("can not kill the service with name %s\n", argv[j]);
                }
            }
        } else if (strcmp(argv[i], "list") == 0) {
            listServices(&services);
        } else if (strcmp(argv[i], "find") == 0) {
            DEBUG
            char* searchName = argv[i + 1];
            _Bool sort = false;
            for (register int j = i + 2; j < argc; j++) {
                if (strcmp(argv[j], "--sort") == 0) {
                    DEBUG
                    sort = true;
                }
            }
            if (__builtin_expect(searchName == NULL, 0)) {
                printf("service name not provideed for search!");
                return 1;
            }
            DEBUG
            search(&services, searchName, sort);
        } else if (strcmp(argv[i], "update") == 0) {
            char* name = argv[i + 1];
            if (__builtin_expect(name == NULL, 0)) {
                fprintf(stderr, "\033[31mservice name is not given to update!\n");
                return 1;
            }
            _Bool stop = false;
            for (register int j = i + 1; j < argc; ++j) {
                if (strcmp(argv[j], "--stop") == 0) {
                    stop = true;
                    break;
                } 
            }
            updateService(&services, name, stop);
        } else if (strcmp(argv[i], "neglect-updates") == 0) {
            neglectUpdate();
            return 0;
        } else if (strcmp(argv[i], "restart") == 0) {
            char* serviceName = argv[i + 1];
            char* bash = argv[i + 2];
            if (__builtin_expect(serviceName == NULL, 0)) {
                fprintf(stderr, "\033[31mserviceName is not given!\n");
                return 1;
            }
            _Bool attach = false;
            for (register int j = i + 1; j < argc; ++j) {
                if (strcmp(argv[j], "--attach") == 0) {
                    attach = true;
                    break;
                }
            }
            if (__builtin_expect(restartService(&services, serviceName, bash, attach) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "rename") == 0) {
            char* oldName = argv[i + 1];
            char* newName = argv[i + 2];
            if (__builtin_expect(isValidName(newName) != 0, 0)) {
                printf("\033[31mRule Violated!\033[0m\n");
                displayServiceRules();
                return 1;
            }
            if (__builtin_expect(oldName == NULL || newName == NULL, 0)) {
                fprintf(stderr, "old/new service name doesn't provided!\n");
                return 1;
            }
            renameService(&services, oldName, newName);
            
        } else if (strcmp(argv[i], "set-limit") == 0) {
            char* serviceName = argv[i + 1];
            if (__builtin_expect(serviceName == NULL, 0)) {
                fprintf(stderr, "service-name expected!\n");
                return 1;
            }
            float cpuLimit = 0.0;
            int errD = 0;
            int memBytes = 0;
            __uint64_t memBytes_lrg = 0;
            char* memBytesStr = NULL;
            _Bool useLarge = false;
            _Bool useExtreme = false;
            __uint128_t memBytesExtr = 0;
            _Bool doCpuLimit = false;
            _Bool doMemLimit = false;
            for (register int j = i; j < argc; ++j) {
                if (strcmp(argv[j], "-cpu") == 0) {
                    char* limitStr = argv[j + 1];
                    if (__builtin_expect(limitStr == NULL, 0)) {
                        fprintf(stderr, "cpu limitation number (from 0 - (you want probably 1 for single for usage) is required!\n");
                        return 1;
                    }
                    cpuLimit = convertToFloat(limitStr, &errD);
                    if (__builtin_expect(errD != 0, 0)) {
                        return 1;
                    }
                    doCpuLimit = true;
                } else if (strcmp(argv[j], "-mem") == 0) {
                    memBytesStr = argv[j + 1];
                    if (__builtin_expect(memBytesStr == NULL, 0)) {
                        fprintf(stderr, "memory limit in size is required either one of ways of doing it [x]k, [x]m, [x]g, [x]t for doing it!\n");
                        return 1;
                    }
                    for (register int k = j; k < argc; ++k) {
                        if (strcmp(argv[k], "-t") == 0 || strcmp(argv[k], "--type") == 0) {
                            char* type = argv[k + 1];
                            if (type == NULL) {
                                fprintf(stderr, "type value required!\n");
                                return 1;
                            }
                            if (strcmp(type, "standard") == 0) {
                                useLarge = false;
                            } else if (strcmp(type, "custom") == 0) {
                                useLarge = true;
                            } else if (strcmp(type, "extreme") == 0) {
                                useExtreme = true;
                            } else {
                                fprintf(stderr, "unknown type \"%s\"!\n", type);
                                displayMemTypeHelp();
                                return 1;
                            }
                        }
                    }
                    doMemLimit = true;
                }
            }
            if (doCpuLimit) {
                max_core_violation(cpuLimit, errD)
                if (__builtin_expect(errD == __MAX_CORE_VIOLATION, 0)) {
                    char conformationToContinue[2];
                    printf("Do you want to continue to use above 10 cores of cpu (y/n)?: ");
                    if(__builtin_expect(scanf("%s", conformationToContinue) != 1, 0)) {
                        perror("corrupted input!\n");
                        return 1;
                    }
                    if (strcmp(conformationToContinue, "n") == 0 || strcmp(conformationToContinue, "N") == 0) {
                        return 1;
                    }
                }
            }
            printf("cpu: %.2f\n", cpuLimit);
            if (doCpuLimit) {
                if (!useLarge && !useExtreme) {
                    if (memBytesStr != NULL) {
                        memBytes = convertToByte(memBytesStr);
                        if (__builtin_expect(memBytes == 0, 0)) {
                            fprintf(stderr, "invalid memory size!\n");
                            displayMemManVerbose();
                            return 1;
                        }
                    }
                    if (__builtin_expect(setCpuResourceLimit(&services, serviceName, cpuLimit, (!!(memBytes)), memBytes) != 0, 0)) {
                        return 1;
                    }
                } else if (useLarge && !useExtreme) {
                    memBytes_lrg = convertToByte_F_LRG(memBytesStr);
                    if (__builtin_expect(memBytes_lrg == 0, 0)) {
                        fprintf(stderr, "invalid memory size!\n");
                        displayMemManVerbose();
                        return 1;
                    }
                    printf("custom mem: %li\n", memBytes_lrg);
                    if (__builtin_expect(setCpuResourceLimit_F_LRG(&services, serviceName, cpuLimit, (!!(memBytes_lrg)), memBytes_lrg) != 0, 0)) {
                        return 1;
                    }
                } else {
                    memBytesExtr = convertToByte_F_EXTR(memBytesStr);
                    if (__builtin_expect(memBytesExtr == 0, 0)) {
                        fprintf(stderr, "invalid memory size!\n");
                        displayMemManVerbose();
                        return 1;
                    }
                    if (__builtin_expect(setCpuResourceLimit_F_EXTR(&services, serviceName, cpuLimit, (!!(memBytesExtr)), memBytesExtr) != 0, 0)) {
                        return 1;
                    }
                }
            } else if (doMemLimit) {
                if (!useLarge && !useExtreme) {
                    if (memBytesStr != NULL) {
                        memBytes = convertToByte(memBytesStr);
                        if (__builtin_expect(memBytes == 0, 0)) {
                            fprintf(stderr, "invalid memory size!\n");
                            displayMemManVerbose();
                            return 1;
                        }
                    }
                    if (__builtin_expect(setMemoryLimit(&services, serviceName, memBytes) != 0, 0)) {
                        return 1;
                    }
                } else if (!useExtreme && useLarge) {
                    printf("this!\n");
                    memBytes_lrg = convertToByte_F_LRG(memBytesStr);
                    printf("mem cus mem: %li\n", memBytes_lrg);
                    if (__builtin_expect(memBytes_lrg == 0, 0)) {
                        fprintf(stderr, "invalid memory size!\n");
                        displayMemManVerbose();
                        return 1;
                    }
                    if (__builtin_expect(setMemoryLimit_F_LRG(&services, serviceName, memBytes_lrg) != 0, 0)) {
                        return 1;
                    }
                } else {
                    memBytesExtr = convertToByte_F_EXTR(memBytesStr);
                    if (__builtin_expect(memBytesExtr == 0, 0)) {
                        fprintf(stderr, "invalid memory size!\n");
                        displayMemManVerbose();
                        return 1;
                    }
                    if (__builtin_expect(setMemoryLimit_F_EXTR(&services, serviceName, memBytesExtr) != 0, 0)) {
                        return 1;
                    }
                }
            }
        } else if (strcmp(argv[i], "res-usage") == 0) {
            char* serviceName = argv[i + 1];
            if (__builtin_expect(serviceName == NULL, 0)) {
                printf("service name i required!\n");
                return 1;
            }
            if (__builtin_expect(showRsUsage(&services, serviceName) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "res-limit") == 0) {
            char* serviceName = argv[i + 1];
            if (__builtin_expect(serviceName == NULL, 0)) {
                fprintf(stderr, "service name required!\n");
                return 1;
            }
            if (__builtin_expect(showRsLimit(&services, serviceName) != 0, 0)) {
                return 1;
            }
        }


    }
    return 0;
}