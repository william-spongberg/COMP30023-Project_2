#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define _POSIX_C_SOURCE 200112L
#define IMAP_PORT "143"

int setup_connection(char *hostname);

#endif