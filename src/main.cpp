#include <iostream>
#include "portscanner/ports.hpp"
#include <sys/socket.h> // socket()
#include <unistd.h>     // close()
#include <netinet/in.h> // sockaddr_in, htons
#include <arpa/inet.h>  // inet_pton

bool is_port_open(sockaddr_in addr, std::uint16_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(port);
    int result = connect(fd, (sockaddr*)&addr, sizeof(addr));
    close(fd);
    return result == 0;
}

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
        if (is_port_open(addr, port))
        {
            std::cout << "Порт " << port << " открыт" << '\n';
        }
        else
        {
            std::cout << "Порт " << port << " закрыт" << '\n';
        }
    }

    return 0;
}