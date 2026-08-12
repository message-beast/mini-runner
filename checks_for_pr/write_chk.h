#include "../basic.h"
#define CHECK_WRITE_PR(fd, _w, len)\
    if (__builtin_expect(_w <= 0 || _w != len, 0)) {\
        perror("write failed!\n");\
        close(fd);\
        exit_program(-1)\
    }
