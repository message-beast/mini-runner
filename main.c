#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
#include "init/job_init.h"
#include "fini/save_jobs.h"
#include "job_ops/free_jobs.h"
#include "job_format/format_time.h"
#include "job_ops/create_job.h"
#include "job_ops/delete_job.h"
#include "job_ops/run_jobs.h"
#include "job_ops/show_jobs.h"
#include "job_res_man/config_res.h"
#include "res_man/utils/helper.h"
#include "res_format/cpu_format.h"
#include "res_format/job_mem_format.h"
#include "exceptions/messages/jobs/help.h"
#include "exceptions/jobs/job_exceptions.h"
#include "daemon_ops/show_limit.h"
#include "daemon_ops/restart_daemon.h"
#include "daemon_ops/stop_daemon.h"
#include "daemon_ops/show_status.h"
#include "io_man/services/apply_backup.h"
#include "io_man/services/create_backup.h"
#include "io_man/jobs/apply_backup.h"
#include "io_man/jobs/create_backup.h"
#include "time_shift/create_time_shift.h"
#include "time_shift/apply_time_shift.h"
#include "env_man/parse.h"
#include "fini/save_parse.h"
#include "env_man/free_env.h"
#include "env_man/add_env.h"
#include "env_man/get_env.h"
#include "env_man/remove_env.h"
#include "env_man/update_env.h"
#include "exceptions/env/env_exceptions.h"
#include "exceptions/messages/env/help.h"
#include "help.h"

#define true 1
#define false 0

DECLARE_128_T

HELP_FUN

struct service** services = NULL;
struct job** jobs = NULL;
_Bool reformatState = true;
struct env** envs = NULL;



__attribute__((constructor))
void init() {
    if (!fileExists("data/updateStatus")) {
        if (initiateMemoryPtr() == 0) {
            DEBUG
            exit_program(-1)
        }
    }
    if (__builtin_expect(services == NULL, 1)) {
        service** tmp = malloc(__INITIAL_SCALE_SIZE_OF_SERVICES__ * sizeof(service*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("memory allocation for services failed!\n");
            exit_program(-1)
        }
        services = tmp;
        tmp = NULL;
    }
    DEBUG
    if(__builtin_expect(loadServices(&services) != 0, 0)) {
        perror("loading service failed!\n");
        exit_program(-1)
    }
    DEBUG
    if (__builtin_expect(jobs == NULL, 1)) {
        job** tmp = malloc(__INITIAL_SCALE_SIZE_OF_JOBS__ * sizeof(service*));
        if (__builtin_expect(tmp == NULL, 0)) {
            perror("memory allocation for jobs failed!\n");
            exit_program(-1)
        }
        jobs = tmp;
        tmp = NULL;
    }
    if(__builtin_expect(loadJobs(&jobs) != 0, 0)) {
        perror("loading job failed!\n");
        exit_program(-1)
    }
    env** tmp = malloc(__INITIAL_SCALE_OF_ENV__ * sizeof(env*));
    if (__builtin_expect(tmp == NULL, 0)) {
        perror("memory allocation for envs failed!\n");
        exit_program(-1)
    }
    envs = tmp;
    tmp = NULL;
    if (__builtin_expect(loadEnvs(&envs) != 0, 0)) {
        perror("environment loading failed!\n");
        exit_program(-1)
    }
    if (__builtin_expect(createBackup() != 0 || createBackupForJobs() != 0, 0)) {
        perror("failed to create time_shift");
        exit_program(-1)
    }
}




__attribute__((destructor))
void closeProcess() {
    #if defined(DEBUG_MODE)
        printf("destructor called!\n");
    #endif
    if (__builtin_expect(reformatState, 1)) {
        if (__builtin_expect(save_services(&services) != 0 || save_jobs(&jobs) != 0, 0)) {
            if (__builtin_expect(applyBackup() != 0 || applyBackupForjobs() != 0, 0)) {
                perror("failed to apply auto time-shift!\n");
                exit_program(-1)
            }
            exit_program(-1)
        }
    }
    if (__builtin_expect(saveEnvs(&envs) != 0, 0)) {
        perror("failed to save envs!\n");
        exit_program(-1)
    }
    freeServices(&services);
    freeJobs(&jobs);
    freeEnvs(&envs);
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
                fprintf(stderr, "\033[31mcan't delete %s\033[0m\n", serviceName);
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
            if (__builtin_expect(runService(&services, &envs, name, bash, attach) != 0, 0)) {
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
            renameService(&services, &envs, oldName, newName);
            
        } else if (strcmp(argv[i], "set-limit") == 0) {
            char* serviceName = argv[i + 1];
            if (__builtin_expect(serviceName == NULL, 0)) {
                fprintf(stderr, "service-name expected!\n");
                return 1;
            }
            double cpuLimit = 0.0;
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
        } else if (strcmp(argv[i], "add-job") == 0) {
            char* jobName = argv[i + 1];
            if (__builtin_expect(jobName == NULL, 0)) {
                fprintf(stderr, "job name is required!\n");
                return 1;
            }
            if (__builtin_expect(!isValidJobName(jobName), 0)) {
                printf("\033[31mRule Violated\033[0m\n");
                displayJobRules();
                return 1;
            }
            char* runnableFile = argv[i + 2];
            if (__builtin_expect(runnableFile == NULL, 0)) {
                fprintf(stderr, "runnable file is required!\n");
                return 1;
            }
            if (__builtin_expect(!isValidJobPath(runnableFile), 0)) {
                fprintf(stderr, "rule violated or %s is not found!\n", runnableFile);
                displayJobRules();
                return 1;
            }
            if (__builtin_expect(!fileExists(runnableFile), 0)) {
                fprintf(stderr, "%s not found!\n", runnableFile);
                return 1;
            }
            char* timeBuff = argv[i + 3];
            if (__builtin_expect(timeBuff == NULL, 0)) {
                fprintf(stderr, "time interval is required!\n");
                return 1;
            }
            __uint64_t timeInSeconds = 0;
            if (__builtin_expect((timeInSeconds = formatTime(timeBuff)) <= 0, 0)) {
                fprintf(stderr, "formating time interval failed!\n");
                return 1;
            }
            if (__builtin_expect(createJob(&jobs, jobName, runnableFile, timeInSeconds) != 0, 0)) {
                return 1;
            }

        } else if (strcmp(argv[i], "remove-job") == 0) {
            char* jobName = argv[i + 1];
            if (__builtin_expect(jobName == NULL, 0)) {
                fprintf(stderr, "job name is required!\n");
                return 1;
            }
            if (__builtin_expect(removeJob(&jobs, jobName) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "warmup-jobs") == 0) {
            if (__builtin_expect(launchDaemon() != 0, 0)) {
                fprintf(stderr, "warming up failed!\n");
                return 1;
            }
            printf("daemon launched!\n");
        } else if (strcmp(argv[i], "list-jobs") == 0) {
            showJobs(&jobs);
        } else if (strcmp(argv[i], "config") == 0) {
            if (strcmp(argv[i + 1], "job") == 0) {
                __uint64_t cpuLimit = 0;
                __uint128_t memLimit = 0;
                for (register int j = i + 1; j < argc; ++j) {
                    if (strcmp(argv[j], "--max-cpu") == 0 || strcmp(argv[j], "-mc") == 0) {
                        char* cpulimitBuff = argv[j + 1];
                        if (__builtin_expect(cpulimitBuff == NULL, 0)) {
                            fprintf(stderr, "cpuLimit range required!\n");
                            return 1;
                        }
                        cpuLimit = convertToSeconds(cpulimitBuff);
                        if (__builtin_expect(cpuLimit == 0, 0)) {
                            fprintf(stderr, "failed to parse cpu core amount or you just passed 0 cores?\n");
                            displayJobRsLimithelp();
                            return 1;
                        }
                    } else if (strcmp(argv[j], "--max-memory") == 0 || strcmp(argv[j], "-mm") == 0) {
                        char* memLimitBuff = argv[j + 1];
                        if (__builtin_expect(memLimitBuff == NULL, 0)) {
                            fprintf(stderr, "memory limit is required!\n");
                            return 1;
                        }
                        memLimit = convertToBytes_JOB(memLimitBuff);
                        if (__builtin_expect(memLimit == 0, 0)) {
                            fprintf(stderr, "failed to parse memory limit or you just passed 0 bytes?\n");
                            displayJobRsLimithelp();
                            return 1;
                        }
                    }
                }
                printf("STAR\n");
                if (__builtin_expect(configJobRes(memLimit, cpuLimit, !!(memLimit), !!(cpuLimit)) != 0, 0)) {
                    return 1;
                }
                printf("FIN\n");
            }
        } else if (strcmp(argv[i], "show-job-limit") == 0) {
            showJobDaemonRsLimits();
        } else if (strcmp(argv[i], "restart-jobs") == 0) {
            if (__builtin_expect(restartJobDaemon(&jobs) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "stop-jobs") == 0) {
            if (__builtin_expect(stopJobDaemon(&jobs) != 0, 0)) {
                return 1;
            }

        } else if (strcmp(argv[i], "job-status") == 0) {
            showDaemonStatus();
        } else if (strcmp(argv[i], "time-shift") == 0) {
            const char* type = argv[i + 1];
            if (__builtin_expect(type == NULL, 0)) {
                fprintf(stderr, "tim eshift operation is required!\n");
                return 1;
            }
            if (strcmp(type, "create") == 0) {
                if (__builtin_expect(createTimeShift() != 0, 0)) {
                    return 1;
                }
            } else if (strcmp(type, "apply") == 0) {
                if (__builtin_expect(applyTimeShift(&services, &jobs) != 0, 0)) {
                    return 1;
                }
                reformatState = false;
            } else {
                printf("unsupported time shift operation %s\n", type);
                return 1;
            }
        } else if (strcmp(argv[i], "add-env") == 0) {
            char* name = argv[i + 1];
            char* key = argv[i + 2];
            char* value = argv[i + 3];
            if (__builtin_expect(name == NULL || key == NULL || value == NULL, 0)) {
                fprintf(stderr, "unexpected format. expected format <name> <key> <value>\n");
                return 1;
            }
            if (__builtin_expect(isValidEnvName(name) == false || isValidEnvKey(key) == false || isValidEnvValue(value) == false, 0)) {
                printf("\033[31mRules violated!\n\033[0m");
                displayEnvHelp();
                return 1;
            }
            if (__builtin_expect(addEnv(&envs, name, key, value) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "show-env") == 0) {
            char* name = argv[i + 1];
            char* key = argv[i + 2];
            if (__builtin_expect(name == NULL || key == NULL, 0)) {
                fprintf(stderr, "unexpected format. expected format <name> <key>\n");
                return 1;
            }
            showEnv(&envs, name, key);
        } else if (strcmp(argv[i], "remove-env") == 0) {
            char* name = argv[i + 1];
            char* key = argv[i + 2];
            if (__builtin_expect(name == NULL || key == NULL, 0)) {
                fprintf(stderr, "unexpected format. expected format <name> <key>\n");
                return 1;
            }
            if (__builtin_expect(removeEnv(&envs, name, key) != 0, 0)) {
                return 1;
            }
        } else if (strcmp(argv[i], "update-env") == 0) {
            char* name = argv[i + 1];
            char* key = argv[i + 2];
            char* value = argv[i + 3];
            if (__builtin_expect(name == NULL || key == NULL || value == NULL, 0)) {
                fprintf(stderr, "unexpected format. expected format <name> <key> <value>\n");
                return 1;
            }
            if (__builtin_expect(isValidEnvName(name) == false || isValidEnvKey(key) == false || isValidEnvValue(value) == false, 0)) {
                printf("\033[31mRules violated!\n\033[0m");
                displayEnvHelp();
                return 1;
            }
            if (__builtin_expect(updateEnv(&envs, name, key, value) != 0, 0)) return 1;
        }


    }
    return 0;
}