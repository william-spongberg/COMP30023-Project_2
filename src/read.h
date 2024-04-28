#ifndef READ_H
#define READ_H

#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void read(int argc, char *argv[], const char *hostname,
          const char *password, const char *folder, int *message_num,
          char *command, char *server_name);

#endif
