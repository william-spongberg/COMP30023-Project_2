#ifndef READ_H
#define READ_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, char **message_num, char **command,
                       char **server_name);

#endif
