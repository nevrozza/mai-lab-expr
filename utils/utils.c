#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void fatal_error(char msg[]) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void soft_error(const char msg[]) {
    // было stderr, но проблемы с CLI
    fprintf(stdout, "Ошибка: %s", msg);
}

void fatal_memory_error() {
    fatal_error("MemoryError");
}