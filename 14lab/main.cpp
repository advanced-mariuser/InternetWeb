#include "src/Core/ProxyServer.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <listening_port> <cache_directory>\n";
        std::cerr << "Example: " << argv[0] << " 8888 ./cache\n";
        return EXIT_FAILURE;
    }

    try
    {
        uint16_t port = std::stoi(argv[1]);
        std::string cacheDir = argv[2];

        ProxyServer server(port, cacheDir);
        server.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
