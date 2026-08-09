#pragma optimize("03")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include "stop_service.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include "../utils.h"
[[nodiscard]] static inline __attribute__((always_inline, hot)) int killService(pid_t pid) {
    if (!pid) {
        return 0;
    }
    if (__builtin_expect(kill(pid, SIGINT) != 0, 0)) {
        fprintf(stderr, "can not kill service with its pid %i\n", pid);
        return -1;
    }
    return 0;
}


__attribute__((hot)) int restartService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ bash, _Bool attach) {
    if (bash == NULL) {
        bash = "run.sh";
    }
    #pragma GCC ivdep
    for (register int i = 0; i < numberOfProjects; ++i) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 128], 0, 3);
        }
        if (__builtin_expect(strcmp((*services)[i]->name, serviceName) == 0, 0)) {
            service* currentService = (*services)[i];
            if (__builtin_expect(killService(currentService->pid) != 0, 0)) {
                return -1;
            }
            if (__builtin_expect(warmService(&((*services)[i]), bash, attach) != 0, 0)) {
                return -1;
            }
            return 0;
        }
    }
    printf("\033[33mcan not find a service with name \"%s\"\n", serviceName);
    return -1;
}