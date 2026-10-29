#include <iostream>
#include <arpa/inet.h>
#include "portscanner/ports.hpp"
#include "portscanner/scanner.hpp"
#include <thread>
#include "portscanner/syn.hpp"
#include "portscanner/socket.hpp"

int main(int argc, char *argv[])
{

    std::cout << "Portscanner v0.1.0" << '\n';

    if (argc != 4)
    {
        std::cout << "Использование: portscanner <цель> -p <порты>" << '\n';
        std::cout << "Пример: portscanner 127.0.0.1 -p 22,80,8000-8100" << '\n';
        return 1;
    }
    else if (std::string(argv[2]) != "-p" && std::string(argv[2]) != "-sS")
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
    std::vector<PortState> results(ports.size());

    const bool syn_mode = std::string(argv[2]) == "-sS";   // SYN-скан?
    if (syn_mode)
    {
        Socket probe(SOCK_RAW, IPPROTO_TCP);                // пробуем взять сырую трубку
        if (probe.fd() < 0)
        {
            std::cout << "Ошибка: для -sS нужен sudo" << '\n';
            return 1;
        }
    }

    const std::size_t workers = 50;   // сколько почтальонов
    std::vector<std::thread> threads; // список почтальонов

    for (std::size_t w = 0; w < workers; w++) // нанимаем по одному
    {
        threads.emplace_back([&, w]() { // записка с заданием:
            for (std::size_t i = w; i < ports.size(); i += workers)
            {
                results[i] = syn_mode ? syn_scan_port(addr, ports[i])
                                      : scan_port(addr, ports[i]); // свои двери: w, w+50, w+100...
            }
        });
    }

    for (auto &t : threads) // ждём, пока все вернутся
    {
        t.join();
    }

    for (std::size_t i = 0; i < ports.size(); ++i)
    {
        switch (results[i])
        {
                case PortState::Open:
            std::cout << "Порт: " << ports[i] << " | Состояние: Открыт";
            {
                std::string banner = syn_mode ? "" : grab_banner(addr, ports[i]);
                if (!banner.empty())
                {
                    std::cout << " | " << banner;
                }
            }
            std::cout << '\n';
            break;
        case PortState::Closed:
            std::cout << "Порт: " << ports[i] << " | Состояние: Закрыт" << '\n';
            break;
        case PortState::Filtered:
            std::cout << "Порт: " << ports[i] << " | Состояние: Фильтруется" << '\n';
            break;
        }
    }

    return 0;
}