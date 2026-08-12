#pragma once
#pragma optimize("O3")
#include <stdio.h>
#include "../base/structure.h"
__attribute__((hot))int restartService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ bash, _Bool attach);