#include <stdio.h>
#include "res_extraction/x86/support.h"
#include "res_extraction/x86/configure_file.h"
#include "res_extraction/data.h"
#include <stdlib.h>
#include "../arch/arch.h"
int main() {
    #if ARCH != ARM_CPU
        makeCpuSelection();
        configureFile();
        free(data);
        data = NULL;
    #endif
    return 0;
}