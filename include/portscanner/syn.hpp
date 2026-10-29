#pragma once

#include <cstdint>
#include <netinet/in.h>

bool send_syn(sockaddr_in addr, std::uint16_t port);