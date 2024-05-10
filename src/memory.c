#include "memory.h"

void check_memory(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
}

void free_memory(FILE *stream, int num_ptrs, ...) {
    va_list args;
    va_start(args, num_ptrs);
    for (int i = 0; i < num_ptrs; i++) {
        free(va_arg(args, void *));
    }
    va_end(args);

    // close file
    if (stream != NULL) {
        fclose(stream);
    }
}