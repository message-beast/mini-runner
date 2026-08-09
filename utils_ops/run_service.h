#pragma once
#pragma optimize("03")
#include "../base/structure.h"
__attribute__((hot)) int runService(service*** __restrict__ services, char* __restrict__ name, char* __restrict__ bash, _Bool attach);
__attribute__((hot)) int warmService(service** __restrict__ services, char* __restrict__ bash, _Bool attach);