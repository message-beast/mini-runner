#pragma once
#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
#include "../base/structure.h"
void listServices(service*** services);
__attribute__((hot)) void search(service*** __restrict__ services, char* __restrict__ serviceNameSearch, _Bool sort);