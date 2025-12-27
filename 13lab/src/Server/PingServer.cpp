#include "PingServer.h"
#include <iostream>
#include <random>
#include <arpa/inet.h>

PingServer::PingServer(uint16_t port, double lossRate)
    : m_port(port), m_lossRate(lossRate)
{
    m_socket.Bind(m_port);
}

void PingServer::Run()
{
    std::cout << "UDP Ping Server is listening on port " << m_port << "\n";
    if (m_lossRate > 0)
    {
        std::cout << "Simulating packet loss with rate: " << m_lossRate * 100 << "%\n";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true)
    {
        try
        {
            sockaddr_in clientAddr;
            auto message = m_socket.RecvFrom(clientAddr);

            if (message)
            {
                if (dis(gen) < m_lossRate)
                {
                    std::cout << "Simulating packet loss for: " << *message << "\n";
                    continue;
                }

                char clientIpStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &clientAddr.sin_addr, clientIpStr, INET_ADDRSTRLEN);
                uint16_t client_port = ntohs(clientAddr.sin_port);

                std::cout << "Received '" << *message << "' from " << clientIpStr << ":" << client_port <<
                    ". Echoing back.\n";
                m_socket.SendTo(*message, clientIpStr, client_port);
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[SERVER ERROR] " << e.what() << std::endl;
        }
    }
}
