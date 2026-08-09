#pragma once
#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
#include "../base/structure.h"
[[nodiscard]]int addProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName);