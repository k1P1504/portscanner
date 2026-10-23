#include "portscanner/scanner.hpp"
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>

class Socket{
    public:
    Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0)) {}
    ~Socket() { if (fd_ >= 0) close(fd_); }

    int fd() const { return fd_; }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    private:
    int fd_;
};

bool is_port_open(sockaddr_in addr, std::uint16_t port)
{
    Socket sock;                    // взяли трубку
    if (sock.fd() < 0)
    {
        return false;               // трубку не дали
    }

        timeval timeout{};
    timeout.tv_sec = 1;                     // ждать 1 секунду
    setsockopt(sock.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    addr.sin_port = htons(port);
    int result = connect(sock.fd(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    return result == 0;
}                                   // ← тут трубка кладётся САМА