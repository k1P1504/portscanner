#pragma once
#include <cerrno>
#include <cstdint>
#include <netinet/in.h>

enum class PortState { Open, Closed, Filtered };

PortState scan_port(sockaddr_in addr, std::uint16_t port);