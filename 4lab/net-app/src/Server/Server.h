#pragma once

#include "../Common/Socket.h"
#include <atomic>
#include <cstdint>
#include <string>

class Server
{
public:
    explicit Server(uint16_t port);
    void Run();

private:
    void HandleClient(Socket socket);
    void Stop();

    uint16_t m_port;
    const std::string m_serverName;
    const int32_t m_serverNumber;

    Socket m_listenerSocket;
    std::atomic<bool> m_shouldStop;
};