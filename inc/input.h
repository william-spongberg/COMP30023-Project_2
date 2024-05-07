#ifndef INPUT_H
#define INPUT_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FORMAT                                                                 \
    "Usage: -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t] " \
    "<command> <server_name> \n"

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, char **str_message_num,
                       int *int_message_num, char **command,
                       char **server_name);

#endif
