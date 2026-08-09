#pragma once
#pragma optimize("03")
#include <stdio.h>
#include "../base/structure.h"
void updateService(service*** __restrict__ services, char* __restrict__ serviceName, _Bool stop);
__attribute__((hot)) void neglectUpdate();