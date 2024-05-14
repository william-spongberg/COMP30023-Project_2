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

    printf(SUBJECT);
    if (subject != NULL) {
        printf(" %s\n", subject);
    } else {
        printf(" %s\n", NO_SUBJECT);
    }

    // free memory
    free_string(from);
    free_string(to);
    free_string(date);
    free_string(subject);
}

void print_header(char *header, char *str) {
    printf("%s", header);
    if (str != NULL)
        printf(" %s\n", str);
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

    // check if next char is aplhanumeric letter (still part of message)
    char *next_start = NULL;
    char *next_end = NULL;
    if (isalpha(*(end + 2)) == 0) {
        next_start = end + 2;
        next_end = strstr(end + 2, "\r\n");
        check_memory(next_end);
    }

    // remove leading whitespace
    while (isspace(*start)) {
        start++;
    }

    // copy message
    char *message = copy_message(start, end, next_start, next_end);
    check_memory(message);

    // remove trailing whitespace
    char *end_of_message = message + strlen(message) - 1;
    while (isspace(*end_of_message)) {
        *end_of_message = '\0';
        end_of_message--;
    }

    return message;
}

char *copy_message(char *start, char *end, char *next_start, char *next_end) {
    char *message = NULL;
    if (next_start == NULL || next_end == NULL) {
        message = (char *)malloc(end - start + 1);
        check_memory(message);
        strncpy(message, start, end - start);
        message[end - start] = '\0';
    } else {
        message = (char *)malloc(next_end - start + 2);
        check_memory(message);
        strncpy(message, start, end - start);
        message[end - start] = '\0';
        strncat(message, next_start, next_end - next_start + 1);
        message[next_end - start + 1] = '\0';
    }

    return message;
}