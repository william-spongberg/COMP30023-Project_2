#include "input.h"
#include <assert.h>

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, char **str_message_num, char **command,
                       char **server_name) {
    int opt;
    while ((opt = getopt(argc, argv, "u:p:f::n::")) != -1) {
        switch (opt) {
        case 'u':
            *username = optarg;
            assert(*username != NULL);
            break;
        case 'p':
            *password = optarg;
            assert(*password != NULL);
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
                strcpy(*str_message_num, optarg);
            } else if (optind < argc && NULL == strchr(argv[optind], '-')) {
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

    // print args not found
    // if (*folder == NULL) {
    //     printf("Folder not found\n");
    // }

    if (*str_message_num == NULL) {
        printf("Message not found\n");
    }

    // (optind = extra args not parsed by getopt)
    // check if all required arguments are present
    if (optind + 2 != argc) {
        fprintf(stderr, FORMAT);
        exit(1);
    }

    // // get command and server name
    *command = argv[optind];
    // printf("command: %s\n", *command);
    *server_name = argv[optind + 1];
    // printf("server_name: %s\n", *server_name);

    // check if command is valid
    if (strcmp(*command, "retrieve") != 0 && strcmp(*command, "parse") != 0 &&
        strcmp(*command, "mime") != 0 && strcmp(*command, "list") != 0) {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }
}
