#include "portscanner/ports.hpp"

#include <charconv>
#include <sstream>

std::optional<std::uint16_t> parse_port(const std::string &text)
{
    int value = 0;
    auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);

    if (ec != std::errc{})
    {

        return std::nullopt;
    }
    if (value < 1 || value > 65535)
    {
        return std::nullopt;
    }
    if (ptr != text.data() + text.size())
    {
        return std::nullopt;
    }

    return static_cast<std::uint16_t>(value);
}

std::vector<std::uint16_t> parse_range(const std::string &text)
{
    auto dash = text.find('-');
    auto left = parse_port(text.substr(0, dash));
    auto right = parse_port(text.substr(dash + 1));

    if (left && right)
    {
        if (*left > *right)
        {
            return {};
        }
        else
        {

            std::vector<std::uint16_t> ports;

            for (int i = *left; i <= *right; i++)
            {
                ports.push_back(static_cast<std::uint16_t>(i));
            }

            return ports;
        }
    }
    else
    {
        return {};
    }
}

std::vector<std::uint16_t> parse_list(const std::string& text) {
    std::vector<std::uint16_t> all;

    std::stringstream tape(text);
    std::string piece;
    while (std::getline(tape, piece, ',')) {
        auto small = parse_range(piece);

        if(small.empty()){
            return {};
        }
        else{
            for(auto port : small) {
                all.push_back(port);
            }
        }
    }
    return all;
}
