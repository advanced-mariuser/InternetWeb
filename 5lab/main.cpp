#include "src/Client/Client.h"
#include "src/Server/Server.h"
#include <iostream>
#include <stdexcept>
#include <string>

void RunServer(uint16_t port)
{
    Server server(port);
    server.Run();
}

void RunClient(const std::string& address, uint16_t port)
{
    Client client(address, port);
    client.Run();
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            throw std::runtime_error(
                    "Usage:\n"
                    "  Server mode: ./net-app <port>\n"
                    "  Client mode: ./net-app <address> <port>\n");
        }

        if (argc == 2)
        {
            int portNum = std::stoi(argv[1]);
            if (portNum <= 1023 || portNum > 65535)
            {
                throw std::runtime_error("Port number must be between 1024 and 65535.");
            }
            RunServer(static_cast<uint16_t>(portNum));
        }
        else if (argc == 3)
        {
            const std::string address = argv[1];
            int portNum = std::stoi(argv[2]);
            if (portNum <= 1023 || portNum > 65535)
            {
                throw std::runtime_error("Port number must be between 1024 and 65535.");
            }
            RunClient(address, static_cast<uint16_t>(portNum));
        }
        else
        {
            throw std::runtime_error("Invalid number of arguments.");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}