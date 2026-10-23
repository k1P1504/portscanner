#pragma once

#include <cstdint>
#include <netinet/in.h>

bool is_port_open(sockaddr_in addr, std::uint16_t port);