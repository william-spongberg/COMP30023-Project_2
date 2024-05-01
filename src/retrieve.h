#ifndef RETRIEVE_H
#define RETRIEVE_H

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define RETRIEVE_CMD "FETCH"
#define RETRIEVE_CMD_SIZE 5
#define RETRIEVE_TAG "A03"
#define RETRIEVE_TAG_SIZE 3
#define BOBY_PEEK "BODY.PEEK[]"
#define BOBY_PEEK_SIZE 11
#define SPACE 1
#define EOL "\r\n"
#define EOL_SIZE 2


/** A function to retrieve the email from the server, only the email specified by the message number is retrieved.
 * Print the email to stdout.
 * Input: int *message_num - pointer to the the sequence ordering of the email to be retrieved, if NULL, retrieve the last email added
 * Return: int - 0 if the email is successfully retrieved, -1 if the email is not successfully retrieved
*/

int retrieve_email(const int *client_socket_fd, int  *msg_num);
char *construct_retrieve_cmd(const int *msg_num_str);

#endif
