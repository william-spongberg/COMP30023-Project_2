#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>

#define LOGIN "LOGIN"
#define LOGOUT "LOGOUT"
#define INBOX "INBOX"
#define SELECT "SELECT"

void login(char *username, char *password, char **tag, char **buffer,
           int connfd, FILE *stream);
void logout(char **tag, char **buffer, int connfd, FILE *stream);
void select_folder(char **str_message_num, char *folder, char **tag,
                   char **buffer, int connfd, FILE *stream);
int verify_login(char *buffer);

#endif
