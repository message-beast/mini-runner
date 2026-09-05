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
    __uint64_t secondsInterval;
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

typedef struct job_rs_limit {
    __uint128_t memoryLimit;
    __uint64_t cpuLimit;
} job_rs_limit;


typedef struct verbose {
    char* name;
    char* githubRepo;
    int pid;
} verbose;

typedef struct param {
    service** service;
    char* command;
    char* name;
    _Bool attach;
} param;

typedef struct env {
    char* name;
    char* key;
    char* value;
} env;

typedef struct mrnExec {
    struct mrnExec* next;
    char** commands;
    int numbers;
    int capacity;
} mrnExec;