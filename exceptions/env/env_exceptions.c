#define true 1
#define false 0

#define has_violated_power_e(str, err)\
char* src = str;\
while (*src) {\
    if (__builtin_expect(*src == '^', 0)) {\
        err = 1;\
        break;\
    }\
    src++;\
}\


#define return_result_e(err)\
if (__builtin_expect(err != 0, 0)) return false;\


#define has_violated_hash_e(str, err)\
char* sr = str;\
while (*sr) {\
    if (__builtin_expect(*sr == '`', 0)) {\
        err = 1;\
        break;\
    }\
    sr++;\
}\



_Bool isValidEnvName(char* name) {
    _Bool err = 0;
    has_violated_hash_e(name, err)
    return_result_e(err)
    has_violated_power_e(name, err)
    return_result_e(err)
    return true;
}

_Bool isValidEnvKey(char* key) {
    _Bool err = 0;
    has_violated_hash_e(key, err)
    return_result_e(err)
    has_violated_power_e(key, err)
    return_result_e(err)
    return true;
}

_Bool isValidEnvValue(char* value) {
    _Bool err = 0;
    has_violated_hash_e(value, err)
    return_result_e(err)
    has_violated_power_e(value, err)
    return_result_e(err)
    return true;
}