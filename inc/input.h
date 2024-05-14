#ifndef INPUT_H
#define INPUT_H

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_NUM_SIZE 100
#define FORMAT                                                                 \
    "Usage: -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t] " \
    "<command> <server_name> \n"

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, char **str_message_num, char **command,
                       char **server_name);

#endif
