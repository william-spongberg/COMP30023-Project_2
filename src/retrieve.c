#include "retrieve.h"
#include "command.h"
#include "memory.h"

#define FETCH "FETCH"
#define BODY "BODY.PEEK[]"
#define BODY_HEADERS "BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)]"

char* retrieve_body(char *str_message_num, char **tag, char **buffer, int connfd,
                  FILE *stream) {
    // get + send command
    char *retrieve_command = create_command(3, FETCH, str_message_num, BODY);
    send_command(retrieve_command, tag, buffer, connfd, stream);

    // skip over the first line
    char *body_start = strchr(*buffer, '\n') + 1;
    check_memory(body_start);

    // end at the last line
    char *body_end = strstr(body_start, "\r\n)\r\n");
    check_memory(body_end);

    // copy the body to a new buffer
    int body_size = body_end - body_start;
    char *body_buffer = (char *)malloc(body_size + 1);
    strncpy(body_buffer, body_start, body_size);
    body_buffer[body_size] = '\0';

    // TODO: verify

    // free memory
    free(retrieve_command);

    return body_buffer;
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
