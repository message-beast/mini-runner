#pragma optimize("O3")
#define _POSIX_C_SOURCE 200809L
#include <sys/stat.h>
#include "utils_ops/load_project.h"
#define true 1
#define false 0


__attribute__((hot)) _Bool fileExists(char* filePath) {
    struct stat st;
    return (stat(filePath, &st) == 0);
}


