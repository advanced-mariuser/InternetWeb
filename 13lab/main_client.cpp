#include "src/Client/PingClient.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <server_host> <server_port>\n";
        return EXIT_FAILURE;
    }

    try
    {
        std::string host = argv[1];
        uint16_t port = std::stoi(argv[2]);

        PingClient client(host, port);
        client.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
