#include "../../base/config.h"
#include "../base.h"



__attribute__((hot)) int createBackupLimit() {
    if (__builtin_expect(doBackup(__FILE_LIMITS, __FILE_LIMITS_COPY) != 0, 0)) return -1;
    return 0;
}