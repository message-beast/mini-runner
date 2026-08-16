#pragma once
#include <stdio.h>
#include <time.h>
#include <signal.h>
typedef struct service {
    char* githubRepo;
    char* name;
    __uint32_t pid;
    _Bool cloned;
} service;

typedef struct job {
    char* name;
    char* runnableFile;
    time_t lastTimeRunned;
    __uint32_t pid;
} job;

typedef struct meminfo {
    char* stack;
    char* heap;
    char* vmrss;
    char* disk;
    int numThreads;
} meminfo;

typedef struct resource {
    meminfo* mem;
    float numOfCores;
} resource;