#include "../base/structure.h"
__attribute__((hot, aligned(64))) envToArg* getArgs(env*** __restrict__ envs, char* __restrict__ name);