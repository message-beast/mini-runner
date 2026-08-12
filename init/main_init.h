#pragma once
#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../basic.h"
#include <string.h>
#include <sys/stat.h>
__attribute__((hot)) int initiateMemoryPtr();
__attribute__((hot)) int loadServices(service*** services);