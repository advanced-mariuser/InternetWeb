#include "Server.h"
#include "../Http/HttpRequest.h"
#include "../Http/HttpResponse.h"
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

Server::Server(uint16_t port, std::string wwwRoot)
    : m_port(port),
      m_wwwRoot(fs::canonical(fs::absolute(wwwRoot)).string()),
      m_shouldStop(false)
{
    m_listenerSocket.Create();
    m_listenerSocket.Bind(m_port);
    m_listenerSocket.Listen();
}

void Server::Run()
{
    std::cout << "[Server] Listening on port " << m_port << ", serving files from '" << m_wwwRoot << "'\n";
    while (!m_shouldStop)
    {
        try
        {
            Socket clientSocket = m_listenerSocket.Accept();
            if (!clientSocket.IsValid()) continue;

            std::jthread(&Server::HandleClient, this, std::move(clientSocket)).detach();
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Server] Error accepting connection: " << e.what() << std::endl;
        }
    }
}

void Server::HandleClient(Socket clientSocket)
{
    std::cout << "[Server] Accepted new connection.\n";
    try
    {
        std::optional<HttpRequest> requestOpt = ReadAndParseRequest(clientSocket);
        if (!requestOpt)
        {
            SendResponse(clientSocket, HttpResponse::StockResponse(400));
            return;
        }

        HttpResponse response = GenerateResponseForRequest(*requestOpt);

        SendResponse(clientSocket, response);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Server] Error handling client: " << e.what() << std::endl;
    }
    std::cout << "[Server] Connection closed.\n\n";
}

std::optional<HttpRequest> Server::ReadAndParseRequest(Socket& clientSocket)
{
    std::vector<char> buffer(4096);
    ssize_t bytesReceived = clientSocket.Recv(buffer.data(), buffer.size() - 1);

    if (bytesReceived <= 0)
    {
        std::cout << "[Server] Client disconnected or empty request.\n";
        return std::nullopt;
    }
    buffer[bytesReceived] = '\0';

    HttpRequest request;
    if (!request.Parse(buffer))
    {
        std::cout << "[Server] Failed to parse request.\n";
        return std::nullopt;
    }

    return request;
}

HttpResponse Server::GenerateResponseForRequest(const HttpRequest& request)
{
    std::cout << "[Server] Received request: GET " << request.GetPath() << "\n";

    std::string path_str = request.GetPath();
    if (path_str == "/")
    {
        path_str = "/index.html";
    }

    fs::path filePath = fs::path(m_wwwRoot) / fs::relative(path_str, "/");
    filePath = fs::weakly_canonical(filePath);

    if (filePath.string().find(m_wwwRoot) != 0)
    {
        std::cout << "[Server] Denied request for forbidden path: " << filePath.string() << "\n";
        return HttpResponse::StockResponse(404);
    }

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> file_buffer(size);
        if (file.read(file_buffer.data(), size))
        {
            HttpResponse response;
            response.SetStatusCode(200, "OK");
            response.SetHeader("Content-Type", GetMimeType(path_str));
            response.SetBody(file_buffer);
            std::cout << "[Server] Responding with 200 OK for: " << filePath.string() << "\n";
            return response;
        }
    }

    std::cout << "[Server] File not found: " << filePath.string() << "\n";
    return HttpResponse::StockResponse(404);
}

void Server::SendResponse(Socket& client_socket, const HttpResponse& response)
{
    std::string response_str = response.BuildResponse();
    client_socket.Send(response_str.c_str(), response_str.length());
}

std::string Server::GetMimeType(const std::string& path)
{
    if (path.ends_with(".html") || path.ends_with(".htm")) return "text/html";
    if (path.ends_with(".css")) return "text/css";
    return "application/octet-stream";
}
