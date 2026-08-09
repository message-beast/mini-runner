#pragma once
#pragma optimize("03")
#pragma target("arch=native")
#include <stdio.h>
#include "../base/structure.h"
__attribute__((hot))int restartService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ bash, _Bool attach);