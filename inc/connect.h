#ifndef CONNECT_H
#define CONNECT_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define IMAP_PORT "143"

int setup_connection(char *hostname);

#endif