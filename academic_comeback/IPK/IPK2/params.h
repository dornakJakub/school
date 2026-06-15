#ifndef PARAMS_H
#define PARAMS_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    SERVER,
    CLIENT,
    UNSPECIFIED
} App_side_type;

typedef enum {
    IPV4,
    IPV6
} Address_type;

typedef struct {
    App_side_type app_type;
    __uint32_t timeout;
    bool show_help;
    char port[6];
    char *address;
    FILE *file;
} Config;

Config *parse_params(int argc, char **argv);

void print_help();

void free_config(Config *conf);

void print_config(Config *conf);

#endif