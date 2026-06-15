#include "params.h"
#include "packet.h"

#define WINDOW_SIZE 10
#define RESEND_TIMEOUT 250000

int init_server(Config *conf);

int init_client(Config *conf);

__uint32_t decode_ack(Packet p);