#include "Client.h"
#include "../Common/Protocol.h"

#include <iostream>
#include <limits>
#include <utility>

Client::Client(std::string address, uint16_t port)
        : m_address(std::move(address)), m_port(port), m_clientName("Client of Ivan Petrov")
{
}

void Client::Run()
{
    int number = 0;
    std::cout << "Please enter an integer [1-100] (or other to stop the server): ";
    std::cin >> number;
    while (std::cin.fail())
    {
        std::cout << "Invalid input. Please enter an integer: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> number;
    }

    try
    {
        Socket socket;
        socket.Create();

        std::cout << "[Client] Connecting to " << m_address << ":" << m_port << "...\n";
        socket.Connect(m_address, m_port);
        std::cout << "[Client] Connection established.\n";

        Protocol::Message clientMessage{ m_clientName, static_cast<int32_t>(number) };
        std::cout << "[Client] Sending data: Name='" << clientMessage.name << "', Number=" << clientMessage.number << "\n";
        Protocol::WriteMessage(socket, clientMessage);

        std::cout << "[Client] Waiting for server response...\n";
        Protocol::Message serverMessage = Protocol::ReadMessage(socket);
        std::cout << "[Client] Response received.\n";

        std::cout << "\n--- RESULTS ---\n";
        std::cout << "  Client Name: " << m_clientName << "\n";
        std::cout << "  Server Name: " << serverMessage.name << "\n";
        std::cout << "  Client Number: " << number << "\n";
        std::cout << "  Server Number: " << serverMessage.number << "\n";
        std::cout << "  Sum: " << (number + serverMessage.number) << "\n";
        std::cout << "---------------\n";

        std::cout << "[Client] Closing connection.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Client] Operation failed: " << e.what() << std::endl;
        throw;
    }
}