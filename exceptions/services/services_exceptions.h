#pragma optimize("fast-math")
#pragma optimize("03")
#pragma target("arch=native")
#include <stdio.h>
#include <string.h>

#define space_available(src, checked)\
char* s = src;\
while(*s) {\
    if (__builtin_expect(*s == ' ', 0)) {\
        checked = 1;\
        break;\
    }\
    s++;\
}\


#define max_char_violation(src, checked, max)\
if (__builtin_expect(strlen(src) > max, 0)) {\
    checked = 1;\
}\

#define return_result(checked)\
if (__builtin_expect(checked == 1, 0)) {\
    return -1;\
}\

#define has_violated_pow(src, checked)\
char* sr = src;\
while(*sr) {\
    if (__builtin_expect(*sr == '^', 0)) {\
        checked = 1;\
        break;\
    }\
    sr++;\
}\


#define has_violated_hash(src, checked)\
char* srr = src;\
while(*srr) {\
    if (__builtin_expect(*srr == '#', 0)) {\
        checked = 1;\
        break;\
    }\
    srr++;\
}\


#define __MAXIMUM_CHAR_FOR_SERVICE_NAME__ 20
#define __MAXIMUM_CHAR_FOR_SERVICE_GITHUB_REPO__ 100


__attribute__((hot)) int isValidName(char* __restrict__ name);
__attribute__((hot)) int isValidGithubRepo(char* __restrict__ name);