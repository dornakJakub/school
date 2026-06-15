#ifndef ERROR_H
#define ERROR_H

#define INTERNAL_ERROR 99
#define INVALID_PARAMETERS 11
#define CONNECTION_ERROR 20
#define TIMEOUT_ERROR 21

void handle_error(int error_code);

#endif