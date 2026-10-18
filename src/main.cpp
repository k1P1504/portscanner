#include <iostream>
#include <cstdint>
#include <optional>
#include <string>
#include <charconv>
#include <vector>

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

int main(int argc, char *argv[])
{

    std::cout << "Portscanner v0.1.0" << '\n';
    std::cout << "аргументов: " << argc << '\n';

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << '\n';
    }

    if (argc >= 2)
    {
        auto ports = parse_range(argv[1]);
        if (ports.empty())
        {
            std::cout << "Ошибка, неверный диапазон" << '\n';
        }
        else
        {
            std::cout << "Всего портов: " << ports.size() << '\n';
        }
    }

    return 0;
}