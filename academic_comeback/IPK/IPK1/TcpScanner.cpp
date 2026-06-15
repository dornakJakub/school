#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   // sockaddr_in, IPPROTO_TCP
#include <netinet/ip.h>   // struct iphdr
#include <netinet/tcp.h>  // struct tcphdr
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // close()
#include <cstring>        // memset
#include <iostream>
#include <string>
#include <stdexcept>
#include "config.hpp"
#include "Port.hpp"


class TcpScanner {
public: 
    TcpScanner(const Config& conf) : timeout(conf.timeout), interface(conf.interface) {
        sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
        if (sock < 0) throw std::runtime_error("Failed to create raw socket, do you have root permissions?\n");

        int one = 1;
        if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
            throw std::runtime_error("Filed to set IP_HDRINCL");
    }

    ~TcpScanner() {
        close(sock);
    }

Port scanPort(const std::string& ip, int port) {
        Port p(port, Protocol::TCP);

        // Build packet
        char packet[4096];
        memset(packet, 0, sizeof(packet));
        struct iphdr* iph = (struct iphdr*)packet;
        struct tcphdr* tcph = (struct tcphdr*)(packet + sizeof(struct iphdr));

        buildIpHeader(iph, interface, ip);
        buildTcpHeader(tcph, 12345, port);

        // Send SYN
        struct sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

        if (sendto(sock, packet, sizeof(struct iphdr) + sizeof(struct tcphdr), 0,
                   (struct sockaddr*)&dest, sizeof(dest)) < 0) {
            perror("sendto");
        }

        // Wait for response
        // TODO: implement libpcap capture to check for SYN+ACK / RST
        bool synAckReceived = false;
        bool rstReceived = false;

        if (synAckReceived) p.setStatus(Status::OPEN);
        else if (rstReceived) p.setStatus(Status::CLOSED);
        else {
            // retry once
            sendto(sock, packet, sizeof(struct iphdr) + sizeof(struct tcphdr), 0,
                   (struct sockaddr*)&dest, sizeof(dest));
            // wait again (placeholder)
            bool retryReceived = false; // placeholder
            if (!retryReceived) p.setStatus(Status::FILTERED);
        }

        return p;
    }

private:
    int sock;
    int timeout;
    std::string interface;

    void buildIpHeader(struct iphdr* iph, const std::string& src, const std::string& dst) {
        iph->ihl = 5;
        iph->version = 4;
        iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
        iph->protocol = IPPROTO_TCP;
        iph->saddr = inet_addr(src.c_str());
        iph->daddr = inet_addr(dst.c_str());
        iph->check = 0;
    }

    void buildTcpHeader(struct tcphdr* tcph, int srcPort, int dstPort) {
        tcph->source = htons(srcPort);
        tcph->dest = htons(dstPort);
        tcph->seq = htonl(0);
        tcph->doff = 5;
        tcph->syn = 1;
        tcph->window = htons(65535);
        tcph->check = 0;
    }

    uint16_t checksum(uint16_t* ptr, int nbytes) {
        long sum;
        uint16_t oddbyte;
        uint16_t answer;

        sum = 0;
        while (nbytes > 1) {
            sum += *ptr++;
            nbytes -= 2;
        }

        if (nbytes == 1) {
            oddbyte = 0;
            *((uint8_t*)&oddbyte) = *(uint8_t*)ptr;
            sum += oddbyte;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = (uint16_t)~sum;

        return answer;
    }
};