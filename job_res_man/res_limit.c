#include <stdio.h>
#include <sys/resource.h>



__attribute__((hot)) int setLimit(__uint128_t memBytes, __uint64_t cpuLimit, _Bool setmemLimit, _Bool setCpuLimit) {
    if (setCpuLimit) {
        const struct rlimit cL = {
            .rlim_cur = cpuLimit,
            .rlim_max = cpuLimit + 3
        };
        if (__builtin_expect(setrlimit(RLIMIT_CPU, &cL) != 0, 0)) {
            perror("failed to set cpu limit!\n");
            return -1;
        }
        if (!setmemLimit) {
            return 0;
        }
    }

    const struct rlimit mL = {
        .rlim_cur = memBytes,
        .rlim_max = memBytes
    };
    if (__builtin_expect(setrlimit(RLIMIT_AS, &mL) != 0, 0)) {
        perror("failed to set memory limit!\n");
        return -1;
    }
    return 0;
}