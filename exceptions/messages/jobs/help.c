#include <stdio.h>




#define NAME_RULES "1. job name must not be greater than 20 characters\n2. job name can not include characters '^', '#', '$', '@'\n"

#define PATH_RULES "1. runnable file path must not be greater than 100 characters\n2. runnable file path can not include characters '^', '#', '$', '@'\n"




#define JOB_RS_FORMAT "1. k -- this represents the memory in kilo bytes\n2. m -- this represents memory in mega bytes\n3. g -- this represents memory in giga bytes\n4. t -- this represents memory in tera bytes\n"

#define JOB_RS_EXAMPLE "\texample: mrn config job --max-cpu 0.7 --max-memory 100k\n\t\t mrn config job --max-cpu 1.2 --max-memory 200m\n\t\t mrn config job --max-cpu 1.7 --max-memory 6g\n\t\t mrn config job --max-cpu 2.3 --max-memory 3t\n"



void displayJobRules() {
    printf("\033[32mJob Name Rules:\n\033[33m%s\033[32mJob Runnable File Rules\n\033[33m%s\033[0m", NAME_RULES, PATH_RULES);
}




void displayJobRsLimithelp() {
    printf("Here is the rule to how you format memory!\n%s%s", JOB_RS_FORMAT, JOB_RS_EXAMPLE);
}