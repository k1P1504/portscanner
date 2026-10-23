#include <iostream>
#include <arpa/inet.h>
#include "portscanner/ports.hpp"
#include "portscanner/scanner.hpp"


int main(int argc, char *argv[])
{

    std::cout << "Portscanner v0.1.0" << '\n';

    if (argc != 4)
    {
        std::cout << "Использование: portscanner <цель> -p <порты>" << '\n';
        std::cout << "Пример: portscanner 127.0.0.1 -p 22,80,8000-8100" << '\n';
        return 1;
    }
    else if (std::string(argv[2]) != "-p")
    {
        std::cout << "Ошибка: неверный аргумент. Используйте -p для указания портов" << '\n';
        return 1;
    }

    auto ports = parse_list(argv[3]);
    if (ports.empty())
    {
        std::cout << "Ошибка: неверные порты" << '\n';
        return 1;
    }
    else
    {
        std::cout << "Цель: " << argv[1] << '\n';
        std::cout << "Всего портов: " << ports.size() << '\n';
    }


    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1)
    {
        std::cout << "Ошибка: неверный IP-адрес" << '\n';
        return 1;
    }
        for (auto port : ports)
    {
        auto state = scan_port(addr, port);
        if (state == PortState::Open)
        {
            std::cout << "Порт " << port << " открыт" << '\n';
        }
        else if (state == PortState::Closed)
        {
            std::cout << "Порт " << port << " закрыт" << '\n';
        }
        else
        {
            std::cout << "Порт " << port << " фильтруется" << '\n';
        }
    }

    return 0;
}