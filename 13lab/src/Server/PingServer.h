#pragma once
#include "../Common/UdpSocket.h"
#include <string>

class PingServer
{
public:
    PingServer(uint16_t port, double lossRate);
    void Run();

private:
    uint16_t m_port;
    double m_lossRate;
    UdpSocket m_socket;
};
