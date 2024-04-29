#ifndef READ_H
#define READ_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_command_line(int argc, char *argv[], const char **username,
                       const char **password, const char **folder,
                       int *message_num, const char **command,
                       const char **server_name);

#endif
