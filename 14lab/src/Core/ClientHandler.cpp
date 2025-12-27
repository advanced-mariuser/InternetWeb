#include "ClientHandler.h"
#include "../Http/HttpRequest.h"
#include <iostream>

ClientHandler::ClientHandler(TcpSocket client_socket, std::shared_ptr<CacheManager> cache_manager)
    : m_clientSocket(std::move(client_socket)), m_cacheManager(std::move(cache_manager))
{
}

void ClientHandler::Handle()
{
    std::cout << "[ClientHandler] New connection accepted\n";
    try
    {
        std::vector<char> buffer(8192);
        ssize_t bytesRead = m_clientSocket.Recv(buffer.data(), buffer.size() - 1);
        if (bytesRead <= 0)
        {
            return;
        }
        buffer.resize(bytesRead);

        HttpRequest request;
        if (!request.Parse(buffer))
        {
            return;
        }

        std::cout << "[ClientHandler] Request for: " << request.GetUrl() << "\n";

        auto cachedResponse = m_cacheManager->Get(request.GetUrl());
        if (cachedResponse)
        {
            std::cout << "[ClientHandler] Cache HIT for " << request.GetUrl() << "\n";
            m_clientSocket.Send(cachedResponse->data(), cachedResponse->size());
        }
        else
        {
            std::cout << "[ClientHandler] Cache MISS for " << request.GetUrl() << "\n";
            std::vector<char> originResponse = ForwardRequestToOrigin(request);

            m_clientSocket.Send(originResponse.data(), originResponse.size());

            m_cacheManager->Put(request.GetUrl(), originResponse);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ClientHandler] Error: " << e.what() << std::endl;
    }
    std::cout << "[ClientHandler] Connection closed\n";
}

std::vector<char> ClientHandler::ForwardRequestToOrigin(const HttpRequest& request)
{
    std::cout << "  -> Connecting to origin server: " << request.GetHost() << ":" << request.GetPort() << "\n";
    TcpSocket originSocket;
    originSocket.Create();
    originSocket.Connect(request.GetHost(), request.GetPort());

    std::string requestStr = request.BuildRequestForTargetServer();
    originSocket.Send(requestStr.c_str(), requestStr.length());

    std::vector<char> response;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = originSocket.Recv(buffer, sizeof(buffer))) > 0)
    {
        response.insert(response.end(), buffer, buffer + bytesRead);
    }

    std::cout << "  -> Received " << response.size() << " bytes from origin\n";
    return response;
}
