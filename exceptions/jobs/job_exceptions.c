#pragma optimize("O3")
#include <stdio.h>
#include <string.h>
#define true 1
#define false 0



#define check_length(name, _flg)\
    if (__builtin_expect(strlen(name) > 20, 0)) {\
        _flg = true;\
    }\


#define check_length_path(name, _flg)\
    if(__builtin_expect(strlen(name) > 100, 0)) {\
        _flg = true;\
    }


#define check_invalid_chars(name, _flg)\
    while(*name) {\
        char c = *name;\
        if (__builtin_expect(c == '^' || c == '#' || c == '$' || c == '@', 1)) {\
            _flg = true;\
            break;\
        }\
        name++;\
    }\


#define return_res(_flg)\
    if (__builtin_expect(_flg, 0)) return false;\




__attribute__((hot)) _Bool isValidJobName(char* name) {
    _Bool error = false;
    check_length(name, error)
    return_res(error)
    check_invalid_chars(name, error)
    return_res(error)
    return true;
}


__attribute__((hot)) _Bool isValidJobPath(char* path) {
    _Bool error = false;
    check_length_path(path, error)
    return_res(error)
    check_invalid_chars(path, error)
    return_res(error)
    return true;
}