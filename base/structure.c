#include <stdio.h>
typedef struct service {
    char* githubRepo;
    char* name;
    __uint32_t pid;
    _Bool cloned;
} service;