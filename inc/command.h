#ifndef COMMAND_H
#define COMMAND_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_DATA_SIZE 4096
#define MAX_LINE_SIZE 1024
#define REALLOC_SIZE 2
#define CARRIAGE_RETURN_NEWLINE "\r\n"
#define CARRIAGE_RETURN_NEWLINE_SIZE 2
#define OK "OK"
#define NO "NO"
#define BAD "BAD"

char *create_command(int num_strs, ...);
char *add_tag(char *tag, char *str);
void send_command(char *command, char **tag, char **buffer, int connfd,
                  FILE *stream);

#endif