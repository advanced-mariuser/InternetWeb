#include "src/Core/Server.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <www-root-directory>\n";
        std::cerr << "Example: " << argv[0] << " 8080 ./www\n";
        return EXIT_FAILURE;
    }

    try
    {
        uint16_t port = std::stoi(argv[1]);
        std::string wwwRoot = argv[2];

        Server server(port, wwwRoot);
        server.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
