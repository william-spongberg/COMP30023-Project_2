#ifndef PARSE_H
#define PARSE_H
#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TO "To:"
#define FROM "From:"
#define DATE "Date:"
#define SUBJECT "Subject:"
#define NO_SUBJECT "<No subject>"

void parse_headers(char *str_message_num, char **tag, char **buffer, int connfd,
                   FILE *stream);
void print_header(char *header, char *str);
char *get_message(char *header);
char *copy_message(char *start, char *end, char *next_start, char *next_end);

#endif
