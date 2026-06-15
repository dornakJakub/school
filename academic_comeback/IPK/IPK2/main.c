#include "params.h"
#include <stdio.h>
#include <stdlib.h>
#include "endpoint.h"

int main(int argc, char **argv) {
    Config *conf = parse_params(argc, argv);
    
    if (conf->show_help) {
        print_help();
        free_config(conf);
        return 0;
    }

    if (conf->app_type == SERVER) {
        init_server(conf);
    } else {
        init_client(conf);
    }

    return 0;
}