#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 200112L

#define PORT "143" // IMAP port

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

    // login to server
    write(connfd, "a LOGIN test@comp30023 pass\n", 29);

    // initialise buffer
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // read response from server
    read(connfd, buffer, 1024);
    printf("%s\n", buffer);

    // access "Test" folder
    write(connfd, "b SELECT Test\n", 15);

    // read response from server
    read(connfd, buffer, 1024);
    printf("%s\n", buffer);

    // close socket
    if (connfd != -1) {
        close(connfd);
    }
    return 0;
}
