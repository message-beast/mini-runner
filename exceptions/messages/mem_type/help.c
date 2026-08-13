#include <stdio.h>


#define TYPE_KINDS "\033[33m1. \"standard\" -- this is best for allocating below 4.2gb\n2. \"custom\" -- this is best for allocating above 4.2gb\033[0m\n"



#define SUPPORTED_PREFIXES "\033[33m\"k\" --this used to declare the memory size in killo bytes\n\"m\" -- this used to declare the memory size in mega bytes\n\"g\" -- this used to declare the memory size in giga bytes\n\"t\" -- this used to declare the memory size in tera bytes\n"

#define PREFIX_EG "--exaple1: mrn set-limit -mem 100k { this sets the memory of 100 kilo bytes }\n--example2: mrn set-limit -mem 200m { this sets the memory of 200 mega bytes }\nexample3: mrn set-limit -mem 4g { this sets the memory size of 4 giga bytes }\n--example4: mrn set-limit -mem 7g --type custom\n"



void displayMemTypeHelp() {
    printf("\033[32mthe supported types for memory allocation are:\n%s", TYPE_KINDS);
}

void displayMemManVerbose() {
    printf("\033[32m Supported prefixes to format a memory size:\n%ssome examples related to memory limitaion:\n%s", SUPPORTED_PREFIXES, PREFIX_EG);
}