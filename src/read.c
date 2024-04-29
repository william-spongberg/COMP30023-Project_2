#include "read.h"
#include <assert.h>

// *TODO*
// fetchmail
// -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]
// <command> <server_name>
// Where <command> may be one of: retrieve, parse, mime, or list

void read_command_line(int argc, char *argv[], char **username, char **password,
                       char **folder, int *message_num, char **command,
                       char **server_name) {
    int opt;
    while ((opt = getopt(argc, argv, "u:p:f:n:")) != -1) {
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
            *folder = optarg;
            assert(*folder != NULL);
            break;
        case 'n':
            *message_num = atoi(optarg);
            break;
            // case 't':
            //     // only specified if using TLS (extension task)
            //     break;
        default:
            fprintf(stderr,
                    "Usage: -u <username> -p <password> [-f <folder>] [-n "
                    "<messageNum>] [-t]\n <command> <server_name> \n");
            exit(1);
        }
    }

    // (optind = extra args not parsed by getopt)
    // check if all required arguments are present
    // TODO: update to allow optional args
    if (optind + 2 != argc) {
        fprintf(stderr, "Usage: -u <username> -p <password> [-f <folder>] [-n "
                        "<messageNum>] [-t]\n <command> <server_name> \n");
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
