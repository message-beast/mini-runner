#include "../base/structure.h"
#pragma once
void listServices(service*** services);
__attribute__((hot)) void search(service*** __restrict__ services, char* __restrict__ serviceNameSearch, _Bool sort);