#pragma once

#include <sys/socket.h>
#include <unistd.h>

class Socket {
    public:
    explicit Socket(int type = SOCK_STREAM, int protocol = 0)
    : fd_(socket(AF_INET, type, protocol)) {}

     ~Socket() {if (fd_ >= 0) close(fd_);}
    int fd() const { return fd_; }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    private:
    int fd_;
};