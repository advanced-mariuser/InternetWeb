#pragma once
#include "../Protocol/UnreliableChannel.h"
#include <string>

class RdtReceiver
{
public:
    RdtReceiver(uint16_t port, std::string filepath, bool debug);
    void ReceiveFile();

private:
    uint16_t m_port;
    std::string m_filepath;
    bool m_debug;
    UnreliableChannel m_channel;
    uint32_t m_expectedSeqNum = 0;
};
