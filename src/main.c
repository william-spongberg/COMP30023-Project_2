#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "parse.h"

#define _POSIX_C_SOURCE 200112L

#define IMAP_PORT "143"
#define MAX_TAG 10000
#define MAX_TAG_SIZE 4
#define MAX_DATASIZE 4096
#define MAX_LINESIZE 1024

void get_tag(char *buffer, size_t size);
void send_command(char *tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(1);
    }

    // *TODO*
    // fetchmail
    // -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]
    // <command> <server_name>
    // Where <command> may be one of: retrieve, parse, mime, or list

    int connfd = 0;
    const char *hostname = argv[1];
    struct addrinfo hints, *result, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    int status = getaddrinfo(hostname, IMAP_PORT, &hints, &result);

    // try to connect to each address in the list
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        // check if hostname is valid
        if (status == 0) {

            // create a socket
            connfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (connfd == -1) {
                perror("socket");
                continue;
            }

            // connect to server through socket
            status = connect(connfd, rp->ai_addr, rp->ai_addrlen);
            if (status == -1) {
                perror("connect");
                fprintf(stderr, "Could not connect to %s\n", hostname);
                exit(2);
            }

            // successfully connected
            printf("Connected to %s\n\n", hostname);
            break;
        } else {
            fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(status));
        }
    }
    if (rp == NULL) {
        fprintf(stderr, "Could not connect to %s\n", hostname);
        exit(2);
    }
    freeaddrinfo(result);

    // initialise stream
    FILE *stream = fdopen(connfd, "r+");

    // initialise buffers
    char *buffer = malloc(MAX_DATASIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
    memset(buffer, 0, MAX_DATASIZE);

    char line[MAX_LINESIZE];
    memset(line, 0, sizeof(line));

    // initialise tag
    char *tag = malloc(MAX_TAG_SIZE);
    if (tag == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(5);
    }
    get_tag(tag, MAX_TAG_SIZE);

    // login command
    char *login = " LOGIN test@comp30023 pass\r\n";
    send_command(tag, login, line, &buffer, connfd, stream);
    memset(buffer, 0, MAX_DATASIZE);

    // select command
    char *select = " SELECT Test\r\n";
    send_command(tag, select, line, &buffer, connfd, stream);
    memset(buffer, 0, MAX_DATASIZE);

    // retrieve command
    char *retrieve = " FETCH 1 BODY.PEEK[]\r\n";
    send_command(tag, retrieve, line, &buffer, connfd, stream);
    memset(buffer, 0, MAX_DATASIZE);

    // parse command
    char *fetch =
        " FETCH 1 BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]\r\n";
    send_command(tag, fetch, line, &buffer, connfd, stream);
    parse_headers(buffer);
    memset(buffer, 0, MAX_DATASIZE);

    // free memory
    free(tag);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }
    return 0;
}

// generate a unique tag that is used to identify each command
// increments the tag each time it is called
void get_tag(char *buffer, size_t size) {
    static int tag = 0;
    if (tag < MAX_TAG) {
        snprintf(buffer, size, "%d", tag++);
    } else {
        fprintf(stderr, "Exceeded maximum number of tags\n");
        exit(5);
    }
}

void send_command(char *tag, char *command, char *line, char **buffer,
                  int connfd, FILE *stream) {
    // initialise command
    get_tag(tag, MAX_TAG_SIZE);
    char *total_command = (char *)malloc(strlen(tag) + strlen(command) + 1);
    strcpy(total_command, tag);
    strcat(total_command, command);

    // send command to server
    write(connfd, total_command, strlen(total_command));
    printf("Sent: %s\n", total_command);

    // receive first response from server
    fgets(line, MAX_DATASIZE, stream);
    strcat(*buffer, line);

    // check if server is ready
    if (strncmp(*buffer, "* ", 2) != 0) {
        fprintf(stderr, "Server not ready\n");
        fprintf(stderr, "Received: %s\n", *buffer);
        exit(3);
    }

    // confirm command
    char *confirm_command = strcat(tag, " OK");
    while (strncmp(line, confirm_command, strlen(confirm_command)) != 0) {
        fgets(line, MAX_DATASIZE, stream);
        strcat(*buffer, line);
        //printf("%s", line);
    }
    printf("Received: %s\n", *buffer);
    printf("\n");
    free(total_command);
}
