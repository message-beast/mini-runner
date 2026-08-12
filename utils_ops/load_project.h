#pragma once
#pragma optimize("O3")
#include "../base/structure.h"
[[nodiscard]]__attribute__((hot)) int loadProject(service*** __restrict__ services, char* __restrict__  githubRepo, char* __restrict__ nickName, pid_t pid);