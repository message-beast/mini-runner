#pragma once
#define _POSIX_C_SOURCE 200809L
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../basic.h"
#include <string.h>
#include <sys/stat.h>
int initiateMemoryPtr();
int loadServices(service*** services);