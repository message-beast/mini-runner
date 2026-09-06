#include <stdio.h>


#define RULES "\033[33m1. all env name, key and values can't have the character \033[31m` \033[33mand \033[31m^\n"

void displayEnvHelp() {
    printf("\033[34mThere is a rule on how to set env!\n%s", RULES);
}