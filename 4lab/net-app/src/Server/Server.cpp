#include "Server.h"
#include "../Common/Protocol.h"

#include <iostream>
#include <thread>
#include <utility>

Server::Server(uint16_t port)
        : m_port(port),
          m_serverName("Server of Ivan Petrov"),
          m_serverNumber(50),
          m_shouldStop(false)
{
    m_listenerSocket.Create();
    m_listenerSocket.Bind(m_port);
    m_listenerSocket.Listen();
}

void Server::Run()
{
    std::cout << "[Server] Started on port " << m_port << ". Name: '" << m_serverName << "', Number: " << m_serverNumber
              << "\n";

    while (!m_shouldStop)
    {
        try
        {
            std::cout << "[Server] Waiting for incoming connections...\n";
            Socket clientSocket = m_listenerSocket.Accept();

            if (!clientSocket.IsValid())
            {
                if (m_shouldStop) break;
                continue;
            }

            std::jthread(&Server::HandleClient, this, std::move(clientSocket)).detach();
        }
        catch (const std::exception& e)
        {
            if (!m_shouldStop)
            {
                std::cerr << "[Server] Accept failed: " << e.what() << std::endl;
            }
        }
    }
    std::cout << "[Server] Shut down gracefully.\n";
}

void Server::HandleClient(Socket clientSocket)
{
    try
    {
        std::cout << "[Server] Accepted connection from a client.\n";

        Protocol::Message clientMessage = Protocol::ReadMessage(clientSocket);

        std::cout << "\n--- [Server] Client Data Received ---\n";
        std::cout << "  Client Name:   " << clientMessage.name << "\n";
        std::cout << "  Server Name:   " << m_serverName << "\n";
        std::cout << "  Client Number: " << clientMessage.number << "\n";
        std::cout << "  Server Number: " << m_serverNumber << "\n";
        std::cout << "  Sum:           " << (clientMessage.number + clientMessage.number) << "\n";
        std::cout << "-------------------------------------\n";

        Protocol::Message serverMessage{m_serverName, m_serverNumber};
        Protocol::WriteMessage(clientSocket, serverMessage);
        std::cout << "[Server] Sent response to client.\n";

        if (clientMessage.number < 1 || clientMessage.number > 100)
        {
            std::cout << "[Server] Received shutdown signal (number " << clientMessage.number
                      << " is out of [1, 100] range). Initiating shutdown...\n";
            Stop();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Server] Error handling client: " << e.what() << std::endl;
    }

    std::cout << "[Server] Connection closed for a client.\n";
}

void Server::Stop()
{
    m_shouldStop = true;
    m_listenerSocket.Close();
}