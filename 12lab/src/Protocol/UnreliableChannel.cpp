#include "UnreliableChannel.h"
#include <random>

UnreliableChannel::UnreliableChannel(double lossRate) : m_lossRate(lossRate)
{
}

void UnreliableChannel::SendTo(const std::vector<char>& data, const std::string& ip, uint16_t port)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    if (dis(gen) < m_lossRate)
    {
        return;
    }
    m_socket.SendTo(data, ip, port);
}

std::vector<char> UnreliableChannel::RecvFrom(sockaddr_in& sourceAddr)
{
    return m_socket.RecvFrom(sourceAddr);
}

UdpSocket& UnreliableChannel::GetSocket()
{
    return m_socket;
}
