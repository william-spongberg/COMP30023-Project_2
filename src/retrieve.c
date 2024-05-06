#include "retrieve.h"
#include "command.h"

#define FETCH "FETCH"
#define BODY "BODY.PEEK[]"
#define BODY_HEADERS "BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]"

int retrieve_body(char *str_message_num, char **tag, char **buffer, int connfd,
                  FILE *stream) {
    // get + send command
    char *retrieve_command = create_command(3, FETCH, str_message_num, BODY);
    send_command(retrieve_command, tag, buffer, connfd, stream);

    // TODO: verify

    // free memory
    free(retrieve_command);
    return 0;
}

int retrieve_headers(char *str_message_num, char **tag, char **buffer,
                     int connfd, FILE *stream) {
    // get + send command
    char *retrieve_headers_command =
        create_command(3, FETCH, str_message_num, BODY_HEADERS);
    send_command(retrieve_headers_command, tag, buffer, connfd, stream);

    // TODO: verify

    // free memory
    free(retrieve_headers_command);
    return 0;
}
