#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void handle_error(int error_code) {
    // switch (error_code) {
    //     case INTERNAL_ERROR:
    //         fprintf(stderr, "Internal error\n");
    //         break;
    //     case INVALID_PARAMETERS:
    //         fprintf(stderr, "Invalid parameters given");
    //     default:
    //         fprintf(stderr, "Unexpected error has occurred\n");
    //         break;
    // }

    exit(error_code);
}