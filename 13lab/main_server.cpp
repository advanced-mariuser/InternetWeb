#include "src/Server/PingServer.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> [loss_rate_percent]\n";
        std::cerr << "Example: " << argv[0] << " 12000\n";
        std::cerr << "Example with 30% loss: " << argv[0] << " 12000 30\n";
        return EXIT_FAILURE;
    }

    try
    {
        uint16_t port = std::stoi(argv[1]);
        double lossRate = (argc == 3) ? (std::stod(argv[2]) / 100.0) : 0.0;

        PingServer server(port, lossRate);
        server.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
