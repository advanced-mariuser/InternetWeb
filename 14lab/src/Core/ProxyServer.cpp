#include "ProxyServer.h"
#include "ClientHandler.h"
#include <iostream>
#include <thread>

ProxyServer::ProxyServer(uint16_t port, const std::string& cacheDir)
    : m_port(port), m_shouldStop(false)
{
    m_cacheManager = std::make_shared<CacheManager>(cacheDir);
    m_listenerSocket.Create();
    m_listenerSocket.Bind(m_port);
    m_listenerSocket.Listen();
}

void ProxyServer::Run()
{
    std::cout << "[ProxyServer] Listening on port " << m_port << ", caching to '"
        << /*cache_dir*/ "some dir" << "'\n";
    while (!m_shouldStop)
    {
        try
        {
            TcpSocket clientSocket = m_listenerSocket.Accept();
            if (!clientSocket.IsValid())
            {
                continue;
            }

            std::jthread([clientSocket = std::move(clientSocket), cache = m_cacheManager]() mutable
            {
                ClientHandler handler(std::move(clientSocket), cache);
                handler.Handle();
            }).detach();
        }
        catch (const std::exception& e)
        {
            std::cerr << "[ProxyServer] Error accepting connection: " << e.what() << std::endl;
        }
    }
}
