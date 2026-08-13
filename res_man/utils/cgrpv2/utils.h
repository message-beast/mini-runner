#define CREATE_PROCESS_REG_F\
    static inline __attribute__((always_inline)) int createProcess(char* serviceName, __uint32_t servicePid) {\
        size_t size = snprintf(NULL, 0, "/sys/fs/cgroup/%s/cgroup.procs", serviceName);\
        if (__builtin_expect(size <= 0, 0)) {\
            perror("fauled to calculate the size of the string of new cgroup process!\n");\
            return -1;\
        }\
        char newPrPath[size + 1];\
        snprintf(newPrPath, sizeof(newPrPath), "/sys/fs/cgroup/%s/cgroup.procs", serviceName);\
        if (__builtin_expect(invalidString(newPrPath), 0)) {\
            perror("failed to create a string of new cgroup process path!\n");\
            return -1;\
        }\
        int fd = open(newPrPath, O_WRONLY);\
        if (__builtin_expect(fd == -1, 0)) {\
            perror("can not open the system cgourp cgroup.procs!\n");\
            return -1;\
        }\
        __asm__ volatile (\
            "mfence"\
            :\
            :\
            : "memory"\
        );\
        size = snprintf(NULL, 0, "%i", servicePid);\
        if (__builtin_expect(size <= 0, 0)) {\
            return -1;\
        }\
        char pidBuff[size + 1];\
        snprintf(pidBuff, sizeof(pidBuff), "%i", servicePid);\
        int wanted_length = strlen(pidBuff);\
        ssize_t written = write(fd, pidBuff, wanted_length);\
        CHECK_WRITE(fd, written, wanted_length);\
        close(fd);\
        return 0;\
    }\


