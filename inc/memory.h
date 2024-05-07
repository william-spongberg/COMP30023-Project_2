#ifndef MEMORY_H
#define MEMORY_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void check_memory(void *ptr);
void free_memory(FILE *stream, int num_ptrs, ...);

#endif