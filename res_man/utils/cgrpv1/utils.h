#define CREATE_PROCESS__F_V1\
    static inline __attribute__((always_inline)) int createProcessV1(char* serviceName, __uint32_t servicePid) {\
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);\
        if (__builtin_expect(size <= 0, 0)) {\
            perror("failed to calculate the service process path string length!\n");\
            return -1;\
        }\
        char serPrPath[size + 1];\
        snprintf(serPrPath, size + 1, "/sys/fs/cgroup/cpu/%s/cgroup.procs", serviceName);\
        if (__builtin_expect(invalidString(serPrPath), 0)) {\
            perror("failed to create service process path string!\n");\
            return -1;\
        }\
        int fd = open(serPrPath, O_WRONLY);\
        if (__builtin_expect(fd == -1, 0)) {\
            perror("failed to open service process file!\n");\
            return -1;\
        }\
        size = snprintf(NULL, 0, "%i", servicePid);\
        if (__builtin_expect(size <= 0, 0)) {\
            perror("failed to calculate the pid buffer string length!\n");\
            return -1;\
        }\
        char pidBuff[size + 1];\
        snprintf(pidBuff, size + 1, "%i", servicePid);\
        if (__builtin_expect(invalidString(pidBuff), 0)) {\
            perror("failed to create pid buffer string!\n");\
            return -1;\
        }\
        int wanted_length = strlen(pidBuff);\
        ssize_t written = write(fd, pidBuff, wanted_length);\
        CHECK_WRITE(fd, written, wanted_length);\
        close(fd);\
        return 0;\
    }\


