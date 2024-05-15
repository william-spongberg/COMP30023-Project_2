#include "input.h"

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, char **str_message_num, char **command,
                       char **server_name) {
    int opt;
    while ((opt = getopt(argc, argv, "u:p:f::n::")) != -1) {
        switch (opt) {
        case 'u':
            *username = optarg;
            break;
        case 'p':
            *password = optarg;
            break;
        case 'f':
            if (optarg) {
                *folder = optarg;
            } else if (optind < argc && NULL == strchr(argv[optind], '-')) {
                *folder = argv[optind++];
            }
            break;
        case 'n':
            if (optarg) {
                if (!(strlen(optarg) < MAX_MESSAGE_NUM_SIZE)) {
                    fprintf(stderr, "Message number too long\n");
                    exit(5);
                }
                strcpy(*str_message_num, optarg);
            } else if (optind < argc && NULL == strchr(argv[optind], '-')) {
                if (!(strlen(argv[optind]) < MAX_MESSAGE_NUM_SIZE)) {
                    fprintf(stderr, "Message number too long\n");
                    exit(5);
                }
                strcpy(*str_message_num, argv[optind++]);
            }
            break;
            // case 't':
            //     // only specified if using TLS (extension task)
            //     break;
        default:
            fprintf(stderr, FORMAT);
            exit(1);
        }
    }

    // (optind = extra args not parsed by getopt)
    // check if arguments are present
    if (optind + 2 != argc) {
        fprintf(stderr, FORMAT);
        exit(1);
    }

    // get command and server name
    *command = argv[optind];
    *server_name = argv[optind + 1];

    // check if command is valid
    if (strcmp(*command, "retrieve") != 0 && strcmp(*command, "parse") != 0 &&
        strcmp(*command, "mime") != 0 && strcmp(*command, "list") != 0) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    // check if all required arguments are present
    if (*username == NULL || *password == NULL || *server_name == NULL) {
        fprintf(stderr, FORMAT);
        exit(1);
    }

}

