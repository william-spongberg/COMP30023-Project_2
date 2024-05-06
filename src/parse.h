#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_headers(char *str_message_num, char **tag, char **buffer, int connfd,
                   FILE *stream);
char *get_message(char *header);

#endif
