#pragma once
#pragma optimize("03")
#pragma target("arch=native")
#include "../base/structure.h"
void renameService(service*** __restrict__ services, char* __restrict__ serviceName, char* __restrict__ newName);