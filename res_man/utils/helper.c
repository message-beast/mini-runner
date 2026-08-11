#define CHECK_WRITE(fd, _w, len)\
    if (__builtin_expect(_w <= 0 || _w != len, 0)) {\
        perror("write failed!\n");\
        close(fd);\
        return -1;\
    }

#define invalidString(x) strlen(x) <= 0