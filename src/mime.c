#include "mime.h"

int fetch_email_body(const int *clinet_socket_fd, const int msg_num) {
    // fetch email body
    char *fetch_cmd = construct_retrieve_cmd(&msg_num);

    int byte_send = send(*clinet_socket_fd, fetch_cmd, strlen(fetch_cmd), 0);

    if (byte_send == -1) {
        perror("send");
        fprintf(stderr, "Failed to send fetch command\n");
        return -1;
    }

    if (byte_send == 0) {
        fprintf(stderr, "Connection closed\n");
        return -1;
    }

    free(fetch_cmd);

    return 0;
}

int get_mime(const int *clinet_socket_fd, const int msg_num) {
    // fetch email body
    int fetch_status = fetch_email_body(clinet_socket_fd, msg_num);
    if (fetch_status == -1) {
        fprintf(stderr, "Failed to fetch email body\n");
        return -1;
    }

    // Receive response from server
    char response[MAX_DATA_SIZE];
    int recv_len;
    recv_len = recv(*clinet_socket_fd, response, MAX_DATA_SIZE, 0); // Read all the response

    // Setup required content type for search
    char *content_type = "Content-Type: text/plain; charset=UTF-8";
    char content_transfer_encoding[][] = {"quoted-printable", "7bit", "8bit"};

    // Boundary to separate the body parts
    char *boundary_header = "boundary=";
    char *boundary_ptr = strstr(response, boundary);
    // Extract the boundary value
    char *boundary_start = boundary_ptr + strlen(boundary_header) + 1;
    char *boundary_end = strchr(boundary_start, '\"');
    char *boundary_value = (char *)malloc(boundary_end - boundary_start + 1);
    strncpy(boundary_value, boundary_start, boundary_end - boundary_start);
    // Add "--" to search for the boundary in the email body
    char *boundary = (char *)malloc(strlen(boundary_value) + 3);
    boundary = strcat("--", boundary_value);

    char *content_type_ptr = strstr(response, content_type);

    if (content_type_ptr == NULL) {
        fprintf(stderr, "Content type text/plain with UTF-8 charset not found\n");
        return -1;
    }

    while(content_type_ptr != NULL) {
        // From the occurrence of matching content type found, search for matching content transfer encoding
        for (int i = 0; i < 3; i++) {
            char *content_transfer_encoding_ptr = strstr(content_type_ptr, content_transfer_encoding[i]);
            if (content_transfer_encoding_ptr != NULL) {
                // Found matching transfer encoding, print the body part separated by "--boundary"
                char *body_end = strstr(content_transfer_encoding_ptr, boundary);
                char *body_start = content_transfer_encoding_ptr;
                char *body = (char *)malloc(body_end - body_start + 1);
                strncpy(body, body_start, body_end - body_start);
                printf("%s\n", body);

                // Free the allocated memory
                free(content_type_ptr);
                free(boundary);
                free(body);
                return 0;
            }

            // If no matching content transfer encoding found, search for the next content type
            content_type_ptr = strstr(content_type_ptr + 1, content_type);
        }
    }

    // Free the allocated memory
    free(content_type_ptr);
    free(boundary);

    // No required content type with the desired charset and transferencoding found
    return -1;
}
