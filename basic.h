#pragma once
#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
__attribute__((hot)) int exit_process(int i);
#define exit_program(i) exit_process(i);
__attribute__((hot)) void cleanupStr(char** ptr);
#define STR __attribute__((cleanup(cleanupStr)))

#if defined(DEBUG_MODE)
    #define DEBUG printf("###DEBUG##### %s: %i\n", __FILE__, __LINE__);
#else
    #define DEBUG 
#endif