#pragma once
#include "../Common/UdpSocket.h"
#include <string>
#include <vector>

class UnreliableChannel
{
public:
    UnreliableChannel(double lossRate);
    void SendTo(const std::vector<char>& data, const std::string& ip, uint16_t port);
    std::vector<char> RecvFrom(sockaddr_in& sourceAddr);
    UdpSocket& GetSocket();

private:
    UdpSocket m_socket;
    double m_lossRate;
};
