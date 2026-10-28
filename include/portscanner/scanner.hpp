#pragma once
#include <cerrno>
#include <cstdint>
#include <netinet/in.h>
#include <string>

enum class PortState { Open, Closed, Filtered };

PortState scan_port(sockaddr_in addr, std::uint16_t port);

std::string grab_banner(sockaddr_in addr, std::uint16_t port);