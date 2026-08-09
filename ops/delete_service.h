#pragma once
#pragma optimize("03")
#pragma optimize("fast-math")
#include "../base/structure.h"
__attribute__((hot)) int normalDeleteServices(service*** __restrict__ services, char* __restrict__ serviceName);
int delete_service_force(service*** __restrict__ services, char* __restrict__ serviceName);