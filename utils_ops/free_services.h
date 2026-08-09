#pragma once
#pragma optimize("03")
#pragma target("arch=native")
#include "../base/structure.h"
__attribute__((hot)) void freeServices(service*** __restrict__ services);