#include <stdio.h>
#include "run/run.h"

int main(int argc, char* argv[]) {
    char* file = argv[1];
    if (__builtin_expect(file == NULL, 0)) {
        fprintf(stderr, "file to run is required!\n");
        return 1;
    }
    if (__builtin_expect(runMrnFile(file) != 0, 0)) {
        return 1;
    }
    return 0;
}