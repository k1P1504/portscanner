#include <iostream>
#include "portscanner/ports.hpp"

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
        auto ports = parse_list(argv[1]);
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