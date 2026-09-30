#include "../../base/config.h"
#include "../base.h"


__attribute__((hot)) int applyBackupLimits() {
    if (__builtin_expect(doBackup(__FILE_LIMITS_COPY, __FILE_LIMITS) != 0, 0)) return -1;
    return 0;
}