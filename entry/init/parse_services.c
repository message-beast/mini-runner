#include "../../init/main_init.h"
#include "../../base/structure.h"
#include "../../base/config.h"
#include "../mm/mman.h"
#include "../../basic.h"

#define NULL (void*)0

struct service** services = NULL;

__attribute__((constructor, no_reoder)) void init() {
    service** tmp = Malloc(sizeof(service*) * __INITIAL_SCALE_SIZE_OF_SERVICES__);
    if (__builtin_expect(tmp == NULL, 0)) {
        exit_program(-1)
    }
    services = tmp;   
}