#include "parse.h"
#include "memory.h"
#include "retrieve.h"

void parse_headers(char *str_message_num, char **tag, char **buffer, int connfd,
                   FILE *stream) {
    // retrieve headers
    retrieve_headers(str_message_num, tag, buffer, connfd, stream);

    // get headers from first occurence of header title
    char *from = get_message(strcasestr(*buffer, FROM));
    char *to = get_message(strcasestr(*buffer, TO));
    char *date = get_message(strcasestr(*buffer, DATE));
    char *subject = get_message(strcasestr(*buffer, SUBJECT));

    // print headers
    print_header(FROM, from);
    print_header(TO, to);
    print_header(DATE, date);
    print_header(SUBJECT, subject);

    // free memory
    free_string(from);
    free_string(to);
    free_string(date);
    free_string(subject);
}

void print_header(char *header, char *str) {
    printf("%s", header);
    if (str != NULL)
        printf("%s\n", str);
    else
        printf("\n");
}

char *get_message(char *header) {
    if (header == NULL) {
        return NULL;
    }

    // remove first word
    char *start = strchr(header, ' ') + 1;
    check_memory(start);

    // remove trailing chars after message
    char *end = strstr(header, "\r\n");
    check_memory(end);

    // if length of the message is 0, return empty string
    if (end - start <= 0) {
        return NULL;
    }

    char *next_start = NULL;
    char *next_end = NULL;

    // check if next char is letter
    if (isalpha(*(end + 2)) == 0) {
        next_start = end + 2;
        next_end = strstr(end + 2, "\r\n");
        check_memory(next_end);
    }

    // copy message
    char *message = copy_message(start, end, next_start, next_end);
    check_memory(message);

    return message;
}

char *copy_message(char *start, char *end, char *next_start, char *next_end) {
    char *message = NULL;
    if (next_start == NULL) {
        message = (char *)malloc(end - start + 1);
        check_memory(message);
        strncpy(message, start, end - start);
        message[end - start] = '\0';
    }
    else {
        message = (char *)malloc(next_end - start + 1);
        check_memory(message);
        strncpy(message, start, end - start);
        if (next_start != NULL && next_end != NULL) {
            strncat(message, next_start, next_end - next_start);
        }
        message[next_end - start] = '\0';
    }

    return message;
}