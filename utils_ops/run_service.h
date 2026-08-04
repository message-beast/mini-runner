#pragma once
#include "../base/structure.h"
int runService(service*** __restrict__ services, char* __restrict__ name, char* __restrict__ bash, _Bool attach);
int warmService(service** __restrict__ services, char* __restrict__ bash, _Bool attach);