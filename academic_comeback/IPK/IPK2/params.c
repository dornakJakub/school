#include "params.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"

void params_err(Config *conf) {
    free_config(conf);
    handle_error(INVALID_PARAMETERS);
}

void expected_arg_err(Config *conf, char option) {
    fprintf(stderr, "Invalid parameters: -%c option expects argument\n", option);
    params_err(conf);
}

void copy_string_param(char** dest, char* src) {
    *dest = (char*)malloc((strlen(src) + 1) * sizeof(char));

    if (dest == NULL) {
        fprintf(stderr, "Memory allocation error");
        handle_error(INTERNAL_ERROR);
    }
    strcpy(*dest, src);
}

Config *parse_params(int argc, char **argv) {
    Config *conf = (Config*)malloc(sizeof(Config));
    if (conf == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        handle_error(INTERNAL_ERROR);
    }
    conf->app_type = UNSPECIFIED;
    conf->address = NULL;
    conf->timeout = 1;
    conf->file = NULL;
    conf->port[0] = '\0';

    char file_flag;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            conf->show_help = true;
        else if (strcmp(argv[i], "-s") == 0) {
            if (conf->app_type != UNSPECIFIED) {
                fprintf(stderr, "Invalid parameters: Application type can be specified only once\n");
                params_err(conf);
            }
            conf->app_type = SERVER;
        } else if (strcmp(argv[i], "-c") == 0) {
            if (conf->app_type != UNSPECIFIED) {
                fprintf(stderr, "Invalid parameters: Application type can be specified only once\n");
                params_err(conf);
            }
            conf->app_type = CLIENT;
        } else if (strcmp(argv[i], "-p") == 0) {
            if (argc - i <= 1) {
                expected_arg_err(conf, 'p');
            }
            i++;
            if (atoi(argv[i]) < 0 || atoi(argv[i]) > 65535) {
                fprintf(stderr, "Invalid parameters: Port number out of bounds\n");
                params_err(conf);
            }
            strcpy(conf->port, argv[i]);
        } else if (strcmp(argv[i], "-a") == 0) {
            if (argc - i <= 1) {
                expected_arg_err(conf, 'a');
            }
            i++;
            if (conf->address == NULL)
                copy_string_param(&(conf->address), argv[i]);
            else {
                fprintf(stderr, "Invalid parameters: -a option can be specified only once\n");
                params_err(conf);
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-i") == 0) {
            char option = argv[i][1];
            if (argc - i <= 1) {
                expected_arg_err(conf, option);
            }
            i++;
            if (conf->file == NULL) {
                if (strcmp(argv[i], "-") == 0)
                    conf->file = option == 'o' ? stdout : stdin;
                else
                    conf->file = fopen(argv[i], option == 'i' ? "rb" : "wb");
                if (conf->file == NULL) {
                    fprintf(stderr, "Error opening file %s\n", argv[i]);
                    free_config(conf);
                    handle_error(INTERNAL_ERROR);
                }
            }
            else {
                fprintf(stderr, "Invalid parameters: -%c option can be specified only once\n", option);
                params_err(conf);
            }

            file_flag = option;
        } else if (strcmp(argv[i], "-w") == 0) {
            if (argc - i <= 1) {
                expected_arg_err(conf, 'w');
            }
            i++;
            conf->timeout = atoi(argv[i]);
        } else {
            fprintf(stderr, "Invalid parameter: %s\n", argv[i]);
            params_err(conf);
        }
    }

    if (conf->app_type == CLIENT && conf->address == NULL) {
        fprintf(stderr, "Invalid parameters: -a option must be specified for client side application\n");
        params_err(conf);
    } else if (conf->app_type == UNSPECIFIED) {
        fprintf(stderr, "Invalid parameters: Type of application must be specified\n");
        params_err(conf);        
    } else if (strlen(conf->port) == 0) {
        fprintf(stderr, "Invalid parameters: port must be specified\n");
        params_err(conf);
    } else if (conf->app_type == CLIENT && file_flag == 'o') {
        fprintf(stderr, "Invalid parameters: -o option is available only for server side application\n");
        params_err(conf);
    }else if (conf->app_type == SERVER && file_flag == 'i') {
        fprintf(stderr, "Invalid parameters: -i option is available only for client side application\n");
        params_err(conf);
    }

    if (conf->file == NULL)
        conf->file = conf->app_type == SERVER ? stdout : stdin;

    // print_config(conf);

    return conf;
}

void free_config(Config *conf) {
    if (conf != NULL) {
        if (conf->file != NULL && (conf->file != stdin || conf->file != stdout))
            fclose(conf->file);
        if (conf->address != NULL)
            free(conf->address);
        free(conf);
    }
}

void print_help() {
    printf("Usage:\n\n");
    printf("./ipk-rdt -c -a HOST -p PORT [-i INPUT] [-w TIMEOUT] [-h | --help]\n\n");
    printf("where:\n");
    printf("\n-h or --help writes usage instructions to stdout and terminates with exit code 0.");
    printf("\n-s starts the receiving side of the application.");
    printf("\n-c starts the sending side of the application.");
    printf("\nExactly one of -c or -s MUST be specified.");
    printf("\n-p PORT specifies the UDP port number.");
    printf("\n-a ADDRESS in server mode specifies the local bind address. If omitted, the server listens on all suitable local addresses.");
    printf("\n-a HOST in client mode specifies the destination hostname or IPv4/IPv6 address. If a hostname resolves to multiple addresses, the implementation MUST attempt at least one of them.");
    printf("\n-i INPUT specifies the input file to send. If omitted or if INPUT is -, the client reads from stdin.");
    printf("\n-o OUTPUT specifies the output file to create or overwrite. If omitted or if OUTPUT is -, the server writes the received data to stdout.");
    printf("\n-w TIMEOUT specifies a positive timeout in whole seconds. If omitted, the value 1 is used.");
    printf("\nAll arguments can be given in any order.\n");
}

void print_config(Config *conf) {
    printf("App type:   %d\n", conf->app_type);
    printf("Timeout:    %d\n", conf->timeout);
    printf("Show help:  %d\n", conf->show_help);
    printf("Port:       %s\n", conf->port);
    printf("Address:    %s\n", conf->address);
}