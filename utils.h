#pragma once
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

int addProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName);
_Bool fileExists(char* filePath);
void freeServices(service*** services);
int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName);