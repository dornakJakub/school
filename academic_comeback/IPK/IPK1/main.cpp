#include <string>
#include <iostream>
#include <getopt.h>
#include "config.hpp"
#include "Port.hpp"
#include "TcpScanner.cpp"
#include <sstream>

void print_help();

std::vector<int> parse_ports(const std::string& str);

int main(int argc, char* argv[]) {
    Config conf;
    try {
        conf = parse_args(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    TcpScanner scanner(conf);

    return 0;
}

Config parse_args(int argc, char* argv[]) {
    Config conf;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hi::t:u:w:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                conf.show_help = true;
                break;

            case 'i':
                if (optarg)
                    conf.interface = optarg;
                else
                    conf.list_interfaces = true;
                break;

            case 't':
                conf.tcp_ports = parse_ports(optarg);
                break;

            case 'u':
                conf.udp_ports = parse_ports(optarg);
                break;

            case 'w':
                conf.timeout = std::stoi(optarg);
                break;

            default:
                throw std::invalid_argument("Invalid arguments");
        }
    }
    
    return conf;
}

void print_help() {
    std::cout <<
        "Usage:\n"
        "./ipk-L4-scan -i INTERFACE [-u PORTS] [-t PORTS] HOST [-w TIMEOUT]\n"
        "\n"
        "Options:\n"
        "  -h, --help        Show this help message\n"
        "  -i INTERFACE      Network interface\n"
        "  -t PORTS          TCP ports\n"
        "  -u PORTS          UDP ports\n"
        "  -w TIMEOUT        Timeout in ms (default 1000)\n";
}

std::vector<int> parse_ports(const std::string& str) {
    std::vector<int> ports;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ',')) {
        int port = std::stoi(token);

        if (port < 1 || port > 65535) {
            throw std::invalid_argument("Invalid port: " + token);
        }

        ports.push_back(port);
    }

    return ports;
}