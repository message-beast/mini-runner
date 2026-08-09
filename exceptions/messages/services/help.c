#pragma target("arch=native")
#pragma optimize("03")
#include <stdio.h>

#define NAME_HELP \
"1. you can't type more than 20 chars for the service name\n2. you can't include the character '#' or '^'\n3. you can't have space in name\n"

#define GITHUB_REPO_HELP \
"1. you can't type more than 100 chars for the service github repo\n2. you can't include the character '#' or '^'\n3. you can't have a space in githubRepo"


void displayServiceRules() {
    printf("\033[32m============== Rules For naming your service ================\033[0m\n");
    printf("%s\n\n", NAME_HELP);
    printf("\033[32m============== Rules For Giving Github Repo =================\033[0m\n");
    printf("%s\n\n", GITHUB_REPO_HELP);
}