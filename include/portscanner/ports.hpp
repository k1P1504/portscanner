#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

std::vector<std::uint16_t> parse_list(const std::string& text);
std::vector<std::uint16_t> parse_range(const std::string &text);
std::optional<std::uint16_t> parse_port(const std::string &text);