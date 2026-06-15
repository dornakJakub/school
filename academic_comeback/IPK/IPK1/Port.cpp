#include <string>
#include "Port.hpp"

class Port {
public:
    Port(int port, Protocol protocol) : port(port), protocol(protocol), status(Status::UNKNOWN) {}

    void setStatus(const Status s) {status = s;}

private:
    int port;
    Protocol protocol;
    Status status;
};