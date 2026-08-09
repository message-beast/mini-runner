#pragma once
#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
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