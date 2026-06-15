#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>
#include <sys/socket.h>
#include "params.h"
#include <stdlib.h>
#include "endpoint.h"
#include <stdio.h>
#include <netinet/in.h>
#include "error.h"
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void teardown(Config *conf, struct addrinfo *address) {
    free_config(conf);
    freeaddrinfo(address);
}

//TODO: resolve '-' to all suitable addresses
struct addrinfo *handle_address(Config *conf) {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = conf->app_type == SERVER ? AI_PASSIVE : 0;

    int status = getaddrinfo(conf->address, conf->port, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        handle_error(INTERNAL_ERROR); //TODO: maybe different error?
    }

    return res;
}

__uint32_t decode_ack(Packet p) {
    if (p.flags & FLAG_ACK) {
        return p.seq;
    }
    return __UINT32_MAX__;
}

void now(struct timeval *t) {
    gettimeofday(t, NULL);
}

int timed_out(struct timeval *sent, int timeout_ms) {
    struct timeval cur;
    gettimeofday(&cur, NULL);

    long elapsed =
        (cur.tv_sec - sent->tv_sec) * 1000 +
        (cur.tv_usec - sent->tv_usec) / 1000;

    return elapsed >= timeout_ms;
}

int init_server(Config *conf) {
    time_t last_progress;
    time(&last_progress);
    struct addrinfo *address = handle_address(conf), *p;

    int sock = -1;

    for (p = address; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0)
            continue;

        int yes = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        if (bind(sock, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(sock);
        sock = -1;
    }
    
    if (sock < 0) {
        fprintf(stderr, "Could not create/connect/bind socket\n");
        teardown(conf, address);
        handle_error(CONNECTION_ERROR);
    }

    int bytes_rx, bytes_tx;
    char buff[BUFFER_SIZE];
    struct sockaddr_storage client_addr;
    Packet pck;

    __uint32_t expected = 0;
    uint32_t window_seq[WINDOW_SIZE];
    Packet window[WINDOW_SIZE];
    int received[WINDOW_SIZE] = {0};
    time_t time_now;

    while (1) {
        time(&time_now);
        if (difftime(time_now, last_progress) >= conf->timeout) {
            teardown(conf, address);
            handle_error(TIMEOUT_ERROR);
        }
        socklen_t addr_len = sizeof(client_addr);
        bytes_rx = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_rx < 0) perror("ERROR: recvfrom");

        __uint32_t received_sum;
        memcpy(&received_sum, buff + CHECKSUM_OFFSET, 4);
        received_sum = ntohl(received_sum);
        memset(buff + CHECKSUM_OFFSET, 0, 4);
        __uint32_t expected_sum = compute_checksum_raw(buff, bytes_rx);
        if (received_sum != expected_sum) {
            continue;
        }

        pck = deserialize_packet(buff);
        if (pck.flags & FLAG_FIN) {
            Packet ack;
            ack.seq = 0;
            ack.len = 0;
            ack.flags = FLAG_FIN;
            char ack_buff[BUFFER_SIZE];
            ack.checksum = 0;
            serialize_packet(ack, ack_buff);
            __uint32_t sum = compute_checksum_raw(ack_buff, HEADER_SIZE);
            uint32_t net_sum = htonl(sum);
            memcpy(ack_buff + CHECKSUM_OFFSET, &net_sum, 4);

            sendto(sock, ack_buff, HEADER_SIZE, 0, (struct sockaddr *)&client_addr, addr_len);

            break;
        }

        Packet ack;
        ack.seq = pck.seq;
        ack.len = 0;
        ack.flags = FLAG_ACK;
        ack.checksum = 0;

        char ack_buff[HEADER_SIZE];
        serialize_packet(ack, ack_buff);
        __uint32_t sum = compute_checksum_raw(ack_buff, HEADER_SIZE);
        uint32_t net_sum = htonl(sum);
        memcpy(ack_buff + CHECKSUM_OFFSET, &net_sum, 4);
        bytes_tx = sendto(sock, ack_buff, HEADER_SIZE, 0, (struct sockaddr *)&client_addr, addr_len);
        if (bytes_tx < 0) perror("ERROR: sendto");

        if (pck.seq >= expected && pck.seq < expected + WINDOW_SIZE) {
            int idx = pck.seq % WINDOW_SIZE;

            if (!received[idx] || window_seq[idx] != pck.seq) {
                window[idx] = pck;
                window_seq[idx] = pck.seq;
                received[idx] = 1;
                time(&last_progress);
            }
        }

        while (received[expected%WINDOW_SIZE]) {
            Packet *in_order = &window[expected % WINDOW_SIZE];
            fwrite(in_order->data, 1, in_order->len, conf->file);
            fflush(conf->file);

            received[expected % WINDOW_SIZE] = 0;
            expected++;
        }
    }

    close(sock);
    teardown(conf, address);
    return 0;
}

int init_client(Config *conf) {
    time_t last_progress;
    time(&last_progress);
    struct addrinfo *address = handle_address(conf), *p;

    int sock = -1;

    for (p = address; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock < 0) {
            sock = -1;
            continue;
        }

        break;       
    }

    if (sock < 0) {
        fprintf(stderr, "Could not create/connect/bind socket\n");
        teardown(conf, address);
        handle_error(CONNECTION_ERROR);
    }

    int bytes_tx, bytes_rx;
    struct sockaddr_storage src;
    socklen_t srclen = sizeof(src);

    char data[PACKET_DATA_SIZE];

    size_t w_base = 0;
    size_t w_next = 0;
    Packet window[WINDOW_SIZE];
    struct timeval sent_time[WINDOW_SIZE];
    int acked[WINDOW_SIZE] = {0};
    time_t time_now;

    while (1) {
        time(&time_now);
        if (difftime(time_now, last_progress) >= conf->timeout) {
            teardown(conf, address);
            handle_error(TIMEOUT_ERROR);
        }

        while (w_next - w_base < WINDOW_SIZE) {
            size_t n = fread(data, 1, sizeof(data), conf->file);
            if (n == 0)
                break;

            Packet pck = build_packet(data, w_next, n);
            char buff[BUFFER_SIZE];
            pck.checksum = 0;
            serialize_packet(pck, buff);
            uint32_t sum = compute_checksum_raw(buff, HEADER_SIZE + pck.len);
            uint32_t net_sum = htonl(sum);
            memcpy(buff + CHECKSUM_OFFSET, &net_sum, 4);

            window[w_next % WINDOW_SIZE] = pck;
            acked[w_next % WINDOW_SIZE] = 0;

            bytes_tx = sendto(sock, buff, pck.len + HEADER_SIZE, 0, p->ai_addr, p->ai_addrlen);
            if (bytes_tx < 0) perror("ERROR: sendto");
            now(&sent_time[w_next % WINDOW_SIZE]);
            w_next++;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock, &fds);

        struct timeval resend_timeout;
        resend_timeout.tv_sec = 0;
        resend_timeout.tv_usec = RESEND_TIMEOUT;
        int ready = select(sock + 1, &fds, NULL, NULL, &resend_timeout);

        if (ready > 0) {
            char buff[BUFFER_SIZE];
            bytes_rx = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr *)&src, &srclen);
            if (bytes_rx < 0) {
                perror("ERROR: recvfrom");
                continue;
            }
            uint32_t received_sum;
            memcpy(&received_sum, buff + CHECKSUM_OFFSET, 4);
            received_sum = ntohl(received_sum);
            memset(buff + CHECKSUM_OFFSET, 0, 4);
            uint32_t expected_sum = compute_checksum_raw(buff, bytes_rx);
            if (received_sum != expected_sum) {
                continue;
            }
            __uint32_t ack = decode_ack(deserialize_packet(buff));
            if (ack >= w_base && ack < w_next && !acked[ack % WINDOW_SIZE]) {
                acked[ack % WINDOW_SIZE] = 1;
                time(&last_progress);
            }
            while (w_base < w_next && acked[w_base % WINDOW_SIZE]) {
                w_base++;
                time(&last_progress);
            }
        }
        for (size_t i = w_base; i < w_next; i++) {
            int idx = i % WINDOW_SIZE;

            if (!acked[idx] && timed_out(&sent_time[idx], RESEND_TIMEOUT)) {

                char buff[BUFFER_SIZE];
                window[idx].checksum = 0;
                serialize_packet(window[idx], buff);
                uint32_t sum = compute_checksum_raw(buff, HEADER_SIZE + window[idx].len);
                uint32_t net_sum = htonl(sum);
                memcpy(buff + CHECKSUM_OFFSET, &net_sum, 4);

                sendto(sock, buff, HEADER_SIZE + window[idx].len, 0, p->ai_addr, p->ai_addrlen);

                now(&sent_time[idx]);
            }
        }

        if (feof(conf->file) && w_base == w_next) {
            Packet fin = build_packet(NULL, 0, 0);
            fin.flags = FLAG_FIN;
            char fb[BUFFER_SIZE];
            fin.checksum = 0;
            serialize_packet(fin, fb);
            uint32_t sum = compute_checksum_raw(fb, HEADER_SIZE);
            uint32_t net_sum = htonl(sum);
            memcpy(fb + CHECKSUM_OFFSET, &net_sum, 4);

            int fin_acked = 0;

            while (!fin_acked) {
                sendto(sock, fb, HEADER_SIZE, 0, p->ai_addr, p->ai_addrlen);
                int bytes = recvfrom(sock, fb, sizeof(fb), 0, NULL, NULL);
                if (bytes > 0) {
                    Packet fp = deserialize_packet(fb);
                    
                    __uint32_t received_sum;
                    memcpy(&received_sum, fb + CHECKSUM_OFFSET, 4);
                    received_sum = ntohl(received_sum);
                    memset(fb + CHECKSUM_OFFSET, 0, 4);
                    __uint32_t expected_sum = compute_checksum_raw(fb, bytes);
                    if (received_sum == expected_sum && fp.flags & FLAG_FIN) {
                        fin_acked = 1;
                    }
                }
            }
            break;
        }
    }

    close(sock);
    teardown(conf, address);
    return 0;
}