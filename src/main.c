#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 200112L

#define PORT "143" // IMAP port
#define MAX_TAG 10000
#define MAX_TAG_SIZE 4
#define MAX_DATASIZE 1024

void get_tag(char *buffer, size_t size);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
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

    int status = getaddrinfo(hostname, PORT, &hints, &result);

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
                continue;
            }

            // successfully connected
            printf("Connected to %s\n", hostname);
            break;
        } else {
            fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(status));
        }
    }
    if (rp == NULL) {
        fprintf(stderr, "Could not connect to %s\n", hostname);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    // initialise tag
    char *tag = malloc(MAX_TAG_SIZE);
    get_tag(tag, MAX_TAG_SIZE);

    // initalise login command
    char *login = " LOGIN test@comp30023 pass\n";    
    char *login_command = (char *)malloc(strlen(tag) + strlen(login) + 1);
    strcpy(login_command, tag);
    strcat(login_command, login);

    // send login command to server
    printf("%s\n", login_command);
    write(connfd, login_command, strlen(login_command));

    // initialise buffer
    char buffer[MAX_DATASIZE];
    memset(buffer, 0, sizeof(buffer));

    // read response from server
    read(connfd, buffer, MAX_DATASIZE);
    printf("%s\n", buffer);

    // initalise select command
    get_tag(tag, MAX_TAG_SIZE);
    char *select = " SELECT Test\n";
    char *select_command = (char *)malloc(strlen(tag) + strlen(select) + 1);
    strcpy(select_command, tag);
    strcat(select_command, select);

    // send select command to server
    printf("%s\n", select_command);
    write(connfd, select_command, strlen(select_command));

    // read response from server
    read(connfd, buffer, MAX_DATASIZE);
    printf("%s\n", buffer);

    // free memory
    free(login_command);
    free(select_command);
    free(tag);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }
    return 0;
}

void get_tag(char *buffer, size_t size) {
    static int tag = 0;
    if (tag < MAX_TAG) {
        snprintf(buffer, size, "%d", tag++);
    } else {
        fprintf(stderr, "Exceeded maximum number of tags\n");
        exit(EXIT_FAILURE);
    }
}
