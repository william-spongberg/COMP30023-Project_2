#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_headers(char *buffer);
char *get_message(char *header);

#endif
