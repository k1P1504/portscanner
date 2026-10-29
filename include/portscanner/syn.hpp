#pragma once

#include "portscanner/scanner.hpp"

#include <cstdint>
#include <netinet/in.h>

PortState syn_scan_port(sockaddr_in addr, std::uint16_t port);
