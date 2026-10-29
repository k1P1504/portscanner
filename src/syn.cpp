#include "portscanner/syn.hpp"
#include "portscanner/socket.hpp"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <chrono>
#include <cstring>

namespace
{
constexpr std::uint16_t kSourcePort = 54321;   // наш «обратный номер»

// «псевдозаголовок»: адреса, которые тоже входят в контрольную сумму
struct PseudoHeader
{
    std::uint32_t src;
    std::uint32_t dst;
    std::uint8_t zero;
    std::uint8_t protocol;
    std::uint16_t tcp_length;
};

// контрольная сумма: складываем по 2 байта, переносы заворачиваем, переворачиваем биты
std::uint16_t checksum(const std::uint8_t* data, std::size_t length)
{
    std::uint32_t sum = 0;
    for (std::size_t i = 0; i + 1 < length; i += 2)
    {
        sum += (data[i] << 8) | data[i + 1];
    }
    if (length % 2 == 1)
    {
        sum += data[length - 1] << 8;
    }
    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return static_cast<std::uint16_t>(~sum);
}

// с какого нашего адреса Linux отправил бы пакет к цели
in_addr source_ip_for(sockaddr_in target)
{
    Socket udp(SOCK_DGRAM);
    target.sin_port = htons(53);
    connect(udp.fd(), reinterpret_cast<sockaddr*>(&target), sizeof(target));
    sockaddr_in local{};
    socklen_t length = sizeof(local);
    getsockname(udp.fd(), reinterpret_cast<sockaddr*>(&local), &length);
    return local.sin_addr;
}
}

PortState syn_scan_port(sockaddr_in addr, std::uint16_t port)
{
    Socket raw(SOCK_RAW, IPPROTO_TCP);             // сырая трубка (нужен sudo)
    if (raw.fd() < 0)
    {
        return PortState::Filtered;
    }

    // заполняем бланк
    tcphdr tcp{};
    tcp.source = htons(kSourcePort);
    tcp.dest = htons(port);
    tcp.seq = htonl(12345);
    tcp.doff = 5;
    tcp.syn = 1;
    tcp.window = htons(1024);

    // считаем контрольную сумму: псевдозаголовок + бланк
    PseudoHeader pseudo{};
    pseudo.src = source_ip_for(addr).s_addr;
    pseudo.dst = addr.sin_addr.s_addr;
    pseudo.protocol = IPPROTO_TCP;
    pseudo.tcp_length = htons(sizeof(tcp));

    std::uint8_t buffer[sizeof(pseudo) + sizeof(tcp)];
    std::memcpy(buffer, &pseudo, sizeof(pseudo));
    std::memcpy(buffer + sizeof(pseudo), &tcp, sizeof(tcp));
    tcp.check = htons(checksum(buffer, sizeof(buffer)));

    // отправляем бланк как есть
    addr.sin_port = 0;
    auto sent = sendto(raw.fd(), &tcp, sizeof(tcp), 0,
                       reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (sent != static_cast<ssize_t>(sizeof(tcp)))
    {
        return PortState::Filtered;
    }

    // слушаем ответы не дольше 1 секунды
    timeval timeout{};
    timeout.tv_sec = 1;
    setsockopt(raw.fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline)
    {
        std::uint8_t packet[1500];
        auto n = recv(raw.fd(), packet, sizeof(packet), 0);
        if (n <= 0)
        {
            break;                                          // тишина — время вышло
        }

        // в начале пакета IP-заголовок, его длина записана в поле ihl
        auto* ip = reinterpret_cast<const iphdr*>(packet);
        std::size_t ip_length = ip->ihl * 4;
        if (static_cast<std::size_t>(n) < ip_length + sizeof(tcphdr))
        {
            continue;                                       // слишком короткий
        }
        auto* reply = reinterpret_cast<const tcphdr*>(packet + ip_length);

        // это ответ именно нам и именно с этого порта?
        if (ip->saddr != addr.sin_addr.s_addr ||
            reply->source != htons(port) ||
            reply->dest != htons(kSourcePort))
        {
            continue;                                       // чужое письмо
        }

        if (reply->syn && reply->ack)
        {
            return PortState::Open;                         // [S.] «давай!»
        }
        if (reply->rst)
        {
            return PortState::Closed;                       // [R] «уходи!»
        }
    }
    return PortState::Filtered;                             // никто не ответил
}
