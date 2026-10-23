#include <iostream>
#include "portscanner/ports.hpp"
#include <sys/socket.h> // socket()
#include <unistd.h>     // close()
#include <netinet/in.h> // sockaddr_in, htons
#include <arpa/inet.h>  // inet_pton

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

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Номер дескриптора: " << fd << '\n';

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ports[0]);
    int ok = inet_pton(AF_INET, argv[1], &addr.sin_addr);
    if (ok != 1)
    {
        std::cout << "Ошибка: неверный IP-адрес" << '\n';
        close(fd);
        return 1;
    }
    else
    {
        int result = connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

        if (result == 0)
        {
            std::cout << "Порт " << ports[0] << " открыт" << '\n';
        }
        else
        {
            std::cout << "Порт " << ports[0] << " закрыт" << '\n';
        }
    }

    close(fd);
    return 0;
}