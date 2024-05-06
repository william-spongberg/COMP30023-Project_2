#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>

#define LOGIN "LOGIN"
#define SELECT "SELECT"

int login(char *username, char *password, char **tag, char **buffer, int connfd,
          FILE *stream);
int select_folder(char *folder, char **tag, char **buffer, int connfd,
                  FILE *stream);
int verify_login(char *tag, char *buffer);
int verify_folder_selection(char *buffer, int *msg_num);

#endif // PROJ2_LIANZHANG2_WSPONGBERG_LOGIN_H
