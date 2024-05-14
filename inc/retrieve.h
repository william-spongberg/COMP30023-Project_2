#ifndef RETRIEVE_H
#define RETRIEVE_H

#include <stdio.h>

char *retrieve_body(char *str_message_num, char **tag, char **buffer,
                    int connfd, FILE *stream);
int retrieve_headers(char *str_message_num, char **tag, char **buffer,
                     int connfd, FILE *stream);

#endif
