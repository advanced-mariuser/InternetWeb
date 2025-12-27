#pragma once
#include "../Common/TcpSocket.h"
#include "../Cache/CacheManager.h"
#include <memory>

class ClientHandler
{
public:
    ClientHandler(TcpSocket clientSocket, std::shared_ptr<CacheManager> cacheManager);
    void Handle();

private:
    std::vector<char> ForwardRequestToOrigin(const class HttpRequest& request);

    TcpSocket m_clientSocket;
    std::shared_ptr<CacheManager> m_cacheManager;
};
