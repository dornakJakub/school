#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct Config
{
    std::string interface;
    std::vector<int> tcp_ports;
    std::vector<int> udp_ports;
    std::string host;
    int timeout = 1000;
    bool show_help = false;
    bool list_interfaces = false;
};

Config parse_args(int argc, char* argv[]);

void print_help();

#endif