#include <stdio.h>
#pragma once
typedef struct service {
    char* githubRepo;
    char* name;
    __uint32_t pid;
    _Bool cloned;
} service;