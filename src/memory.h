#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

void check_memory(void *ptr);
void free_memory(int connfd, FILE *stream, int num_ptrs, ...);

#endif