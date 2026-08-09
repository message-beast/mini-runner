#pragma once
#pragma optimize("03")
#pragma target("arch=native")
#include "../base/structure.h"
int stopService(service*** __restrict__ services, char* __restrict__ serviceName);