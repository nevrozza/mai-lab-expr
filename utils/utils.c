#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void fatal_error(char msg[]) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void fatal_memory_error() {
    fatal_error("MemoryError");
}