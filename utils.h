#pragma once
#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "base/structure.h"
#include "base/config.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "basic.h"
#include <sys/wait.h>
#include <threads.h>
#include "utils_ops/load_project.h"
#include "utils_ops/free_services.h"
#include "utils_ops/load_project.h"
#include "utils_ops/run_service.h"
int addProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName);
__attribute__((hot)) _Bool fileExists(char* filePath);
void freeServices(service*** __restrict__ services);
int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName, pid_t pid);
int runService(service*** __restrict__ services, char* __restrict__ name, char* __restrict__ bash, _Bool attach);
int warmService(service** __restrict__ services, char* __restrict__ bash, _Bool attach);