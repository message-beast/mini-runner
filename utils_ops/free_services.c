#pragma optimize("O3")
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
__attribute__((hot)) void freeServices(service*** __restrict__ services) {
    if (__builtin_expect(services == NULL || (*services) == NULL, 0)) {
        return;
    }
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((i & 63) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 64], 0, 3);
        }
        if (__builtin_expect((*services)[i] != NULL, 1)) {
            service* curSer = (*services)[i];
            free(curSer->githubRepo);
            free(curSer->name);
            free(curSer);
            curSer = NULL;
        }
    }
    if (__builtin_expect(*services != NULL, 1)) {
        free(*services);
    }
    *services = NULL;
    numberOfProjects = 0;
    capacityOfServices = 0;
}