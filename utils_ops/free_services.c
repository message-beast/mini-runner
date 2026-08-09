#pragma optimize("03")
#pragma target("arch=native")
#include "../base/structure.h"
#include "../base/config.h"
#include <stdlib.h>
__attribute__((hot)) void freeServices(service*** __restrict__ services) {
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