#include "read.h"

void read(int argc, char *argv[], char *hostname, char *password, char *folder,
          int *message_num, char *command, char *server_name) {
    int opt;
    while (opt = getopt(argc, argv, "h:p:f:m:c:s:"), opt != -1) {
        switch (opt) {
        case 'h':
            strcpy(hostname, optarg);
            break;
        case 'p':
            strcpy(password, optarg);
            break;
        case 'f':
            strcpy(folder, optarg);
            break;
        case 'n':
            *message_num = atoi(optarg);
            break;
        case 't':
            // FIXME: Need to figure out what the input format is for this tag
        default:
            fprintf(stderr,
                    "Usage: %s -h <hostname> -p <password> -f <folder> -n "
                    "<message_num> -c <command> -s <server_name>\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}