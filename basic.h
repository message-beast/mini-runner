#pragma once
#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
int exit_process(int i);
#define exit_program(i) exit_process(i);
#define STR