#pragma once
#include "../Common/UdpSocket.h"
#include <string>
#include <chrono>
#include <vector>
#include <limits>

class PingClient
{
private:
    struct Statistics
    {
        int packets_sent = 0;
        int packets_received = 0;
        double min_rtt = std::numeric_limits<double>::max();
        double max_rtt = 0.0;
        double total_rtt = 0.0;

        void Update(double rtt);
        void PrintSummary() const;
    };

public:
    PingClient(std::string host, uint16_t port);
    void Run();

private:
    std::string m_host;
    uint16_t m_port;
    UdpSocket m_socket;
    Statistics m_stats;
};
