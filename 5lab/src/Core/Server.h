#pragma once
#include "../Common/Socket.h"
#include "../Http/HttpRequest.h"
#include "../Http/HttpResponse.h"
#include <string>
#include <atomic>
#include <optional>

class Server
{
public:
    Server(uint16_t port, std::string wwwRoot);
    void Run();

private:
    void HandleClient(Socket clientSocket);
    static std::string GetMimeType(const std::string& path);

    static std::optional<HttpRequest> ReadAndParseRequest(Socket& clientSocket);
    HttpResponse GenerateResponseForRequest(const HttpRequest& request);
    static void SendResponse(Socket& client_socket, const HttpResponse& response);

    uint16_t m_port;
    std::string m_wwwRoot;
    Socket m_listenerSocket;
    std::atomic<bool> m_shouldStop;
};
