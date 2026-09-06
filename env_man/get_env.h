#include "../base/structure.h"
__attribute__((hot, aligned(64))) envGroup* getEnvs(env*** __restrict__ envs, char* __restrict__ name);
void showEnv(env*** __restrict__ envs, char* __restrict__ name, char* __restrict__ key);