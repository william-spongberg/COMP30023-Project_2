#ifndef READ_H
#define READ_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_user_input(int argc, char *argv[], char *hostname, char *password, char *folder,
          int *message_num, char *command, char *server_name);

#endif