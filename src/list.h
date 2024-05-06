#ifndef LIST_H
#define LIST_H

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define LIST_CMD "FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n"
#define LIST_CMD_LEN strlen(LIST_CMD)
#define MAX_DATA_SIZE 4096
#define LIST_TAG "A03"

/**
 * Prints all subject lines of all emails in the mailbox
 * sorted by message sequence number (msg_seq_num)
 * prints nothing on empty mailbox and returns 0
 * ASSUMES at login the appropriate mailbox is already selected
 *
 * return 1 if mail box is empty
 * return 0 if successful
 * return -1 if error
 * */

int list_emails(const int *client_socket_fd, char *folder_name);

#endif
