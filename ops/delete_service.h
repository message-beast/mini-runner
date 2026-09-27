#pragma once
#include "../base/structure.h"
__attribute__((hot)) int normalDeleteServices(service*** __restrict__ services, limit*** __restrict__ limits, char* __restrict__ serviceName);
int delete_service_force(service*** __restrict__ services, char* __restrict__ serviceName);