#ifndef LOAD_ENV_H
    #define LOAD_ENV_H
#endif
#include "../base/structure.h"
__attribute__((hot, aligned(64))) int loadEnv(env*** __restrict__ __env, env* __restrict__ newEnv);