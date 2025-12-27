#pragma once
#include "../Common/TcpSocket.h"
#include "../Cache/CacheManager.h"
#include <string>
#include <atomic>
#include <memory>

class ProxyServer
{
public:
    ProxyServer(uint16_t port, const std::string& cacheDir);
    void Run();

private:
    uint16_t m_port;
    TcpSocket m_listenerSocket;
    std::shared_ptr<CacheManager> m_cacheManager;
    std::atomic<bool> m_shouldStop;
};
