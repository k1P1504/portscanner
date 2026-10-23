#include "portscanner/scanner.hpp"

#include <sys/socket.h>
#include <unistd.h>


bool is_port_open(sockaddr_in addr, std::uint16_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(port);
    int result = connect(fd, (sockaddr*)&addr, sizeof(addr));
    close(fd);
    return result == 0;
}