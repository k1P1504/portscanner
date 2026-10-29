#include "portscanner/scanner.hpp"
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include "portscanner/socket.hpp"

PortState scan_port(sockaddr_in addr, std::uint16_t port)
{
    Socket sock; // взяли трубку
    if (sock.fd() < 0)
    {
        return PortState::Closed; // трубку не дали
    }

    timeval timeout{};
    timeout.tv_sec = 1; // ждать 1 секунду
    setsockopt(sock.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    addr.sin_port = htons(port);
    int result = connect(sock.fd(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (result == 0)
    {
        return PortState::Open;
    }
    else
    {
        if (errno == ECONNREFUSED)
        {
            return PortState::Closed;
        }
        else
        {
            return PortState::Filtered;
        }
    }
} // ← тут трубка кладётся САМА

std::string grab_banner(sockaddr_in addr, std::uint16_t port)
{
    Socket sock; // взяли трубку
    if (sock.fd() < 0)
    {
        return "";
    }

    timeval timeout{};
    timeout.tv_sec = 1;
    setsockopt(sock.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)); // ждать звонка 1с
    setsockopt(sock.fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // ждать ответа 1с

    addr.sin_port = htons(port);
    if (connect(sock.fd(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
    {
        return ""; // не дозвонились
    }

    char buffer[256]{};                                  // блокнот на 256 букв
    auto n = recv(sock.fd(), buffer, sizeof(buffer), 0); // слушаем
    if (n <= 0)
    {
        // молчит — задаём вопрос как веб-браузер
        const std::string question = "HEAD / HTTP/1.0\r\n\r\n";
        send(sock.fd(), question.data(), question.size(), 0); // говорим

        n = recv(sock.fd(), buffer, sizeof(buffer), 0); // слушаем снова
        if (n <= 0)
        {
            return ""; // всё равно молчит
        }

        std::string reply(buffer, n);
        auto pos = reply.find("Server: "); // ищем табличку
        if (pos == std::string::npos)
        {
            return reply.substr(0, reply.find_first_of("\r\n")); // нет таблички — первая строка
        }
        pos += 8; // пропустить само слово "Server: "
        return reply.substr(pos, reply.find_first_of("\r\n", pos) - pos);
    }

    std::string banner(buffer, n);                         // n букв из блокнота → строка
    return banner.substr(0, banner.find_first_of("\r\n")); // только первая строка
}