#include "PingClient.h"
#include <iostream>
#include <fmt/core.h>
#include <thread>

void PingClient::Statistics::Update(double rtt)
{
    packets_received++;
    total_rtt += rtt;
    if (rtt < min_rtt)
    {
        min_rtt = rtt;
    }
    if (rtt > max_rtt)
    {
        max_rtt = rtt;
    }
}

void PingClient::Statistics::PrintSummary() const
{
    int lost = packets_sent - packets_received;
    double lossPercentage = (packets_sent > 0) ? (static_cast<double>(lost) / packets_sent * 100.0) : 0.0;
    double avgRtt = (packets_received > 0) ? (total_rtt / packets_received) : 0.0;

    std::cout << "\n--- Статистика ping ---\n";
    std::cout << fmt::format("Отправлено: {}, Получено: {}, Потеряно: {} ({:.1f}%)\n",
                             packets_sent, packets_received, lost, lossPercentage);
    if (packets_received > 0)
    {
        std::cout << fmt::format("RTT: мин = {:.3f}с, макс = {:.3f}с, средн = {:.3f}с\n",
                                 min_rtt, max_rtt, avgRtt);
    }
}


PingClient::PingClient(std::string host, uint16_t port)
    : m_host(std::move(host)), m_port(port)
{
    m_socket.SetRecvTimeout(1, 0);
}

void PingClient::Run()
{
    const int numPings = 10;
    std::cout << "Pinging " << m_host << " with 10 UDP packets:\n";

    for (int i = 0; i < numPings; ++i)
    {
        m_stats.packets_sent++;

        long long timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::string message = fmt::format("Ping {} {}", i + 1, timestampMs);

        auto startTime = std::chrono::high_resolution_clock::now();
        m_socket.SendTo(message, m_host, m_port);

        sockaddr_in serverAddr;
        auto response = m_socket.RecvFrom(serverAddr);
        auto endTime = std::chrono::high_resolution_clock::now();

        if (response)
        {
            std::chrono::duration<double> rtt = endTime - startTime;
            std::cout << fmt::format("Ответ от сервера: {}, RTT = {:.3f} сек\n", *response, rtt.count());
            m_stats.Update(rtt.count());
        }
        else
        {
            std::cout << "Request timed out.\n";
        }

        if (i < numPings - 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    m_stats.PrintSummary();
}
