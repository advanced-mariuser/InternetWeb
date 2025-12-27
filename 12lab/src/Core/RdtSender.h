#pragma once
#include "../Protocol/UnreliableChannel.h"
#include "../Protocol/Packet.h"
#include <string>
#include <vector>
#include <chrono>

class RdtSender
{
public:
    RdtSender(std::string host, uint16_t port, bool debug);
    void SendFile(const std::string& filepath);

private:
    std::string m_host;
    uint16_t m_port;
    bool m_debug;

    UnreliableChannel m_channel;

    uint32_t m_base = 0;
    uint32_t m_nextSeqNum = 0;
    size_t m_windowSize = 10;
    std::chrono::milliseconds m_timeout{1000};
    std::vector<Packet> m_sentPackets;
};
