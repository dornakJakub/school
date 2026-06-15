#include "params.h"

#define PACKET_DATA_SIZE 1024
#define BUFFER_SIZE 1200
#define HEADER_SIZE 12
#define CHECKSUM_OFFSET 8

#define FLAG_ACK 1
#define FLAG_DATA 0
#define FLAG_FIN 2

typedef struct {
    char data[PACKET_DATA_SIZE];
    __uint32_t seq;
    __uint16_t len;
    __uint16_t flags;
    __uint32_t checksum;
} Packet;

Packet build_packet(char msg[], __uint32_t seq_num, __uint16_t len);

void serialize_packet(Packet p, char* buff);

Packet deserialize_packet(char *buff);

__uint32_t compute_checksum_raw(const void *data, size_t len);