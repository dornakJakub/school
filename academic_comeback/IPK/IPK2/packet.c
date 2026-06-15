#include "packet.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

Packet build_packet(char data[], __uint32_t seq_num, __uint16_t len) {
    Packet p = {0};

    if (len > PACKET_DATA_SIZE)
        len = PACKET_DATA_SIZE;

    memcpy(p.data, data, len);
    p.seq = seq_num;
    p.len = len;
    p.flags = 0;
    p.checksum = 0;

    return p;
}

void serialize_packet(Packet p, char *buff) {
    
    __uint32_t seq_net = htonl(p.seq);
    __uint16_t len_net = htons(p.len);
    __uint16_t flags_net = htons(p.flags);
    __uint32_t checksum_net = htonl(p.checksum);

    memcpy(buff, &seq_net, 4);
    memcpy(buff + 4, &len_net, 2);
    memcpy(buff + 6, &flags_net, 2);
    memcpy(buff + 8, &checksum_net, 4);
    memcpy(buff + HEADER_SIZE, p.data, p.len);
}

Packet deserialize_packet(char *buff) {
    Packet p;

    memcpy(&p.seq, buff, 4);
    memcpy(&p.len, buff + 4, 2);
    memcpy(&p.flags, buff + 6, 2);
    memcpy(&p.checksum, buff + 8, 4);

    p.seq = ntohl(p.seq);
    p.len = ntohs(p.len);
    p.flags = ntohs(p.flags);
    p.checksum = ntohl(p.checksum);

    memcpy(&p.data, buff + HEADER_SIZE, p.len);

    return p;
}

uint32_t compute_checksum_raw(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t sum = 0;

    for (size_t i = 0; i < len; i++) {
        sum += bytes[i];
    }

    return sum;
}