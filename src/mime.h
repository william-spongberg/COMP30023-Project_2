#ifndef MIME_H
#define MIME_H

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "retrieve.h"

#define MIME_TAG "A04"
#define MIME_TAG_SIZE 3
#define MIME_CMD "FETCH"
#define MIME_CMD_SIZE 5
#define MIME_BODY "BODY[]"
#define MIME_BODY_SIZE 6

// GOAL: find the first UTF-8 text/plain body part of an email

// Fetch body of email

// identify MIME header
    // Version (Always 1.0 for this project)
    // Content type (Always text/plain for this project) ; boundary (AKA delimiter)

// Within the body, find the parts separated by the boundary and satisfy all of the following condition:
    // Content type is text/plain
    // Charset is UTF - 8
    // Content transfer encoding quoted-printable | 7bit | 8bit

int get_mime(const int *client_socket_fd, char *msg_num);
#endif
