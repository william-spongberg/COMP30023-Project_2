#ifndef PROJ2_LIANZHANG2_WSPONGBERG_LOGIN_H
#define PROJ2_LIANZHANG2_WSPONGBERG_LOGIN_H

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/** A function to send a message to the server to request a logon, return
 * 0 if the logon is successful, -1 if the logon is unsuccessful.
 * INPUT: socket_fd - the client socket file descriptor
 *       username - the username to logon with
 *       password - the password to logon with
 * RETURN: 0 if the logon is successful, -1 if the logon is unsuccessful
 */
int login(const int *client_socket_fd, char *username, char *password);
int verify_login(const int *client_socket_fd);

#endif // PROJ2_LIANZHANG2_WSPONGBERG_LOGIN_H
