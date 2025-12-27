#include "src/Core/RdtSender.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <receiver_host> <receiver_port> <file_to_send> [-d]\n";
        return EXIT_FAILURE;
    }
    try
    {
        std::string host = argv[1];
        uint16_t port = std::stoi(argv[2]);
        std::string filepath = argv[3];
        bool debug = (argc == 5 && std::string(argv[4]) == "-d");

        RdtSender sender(host, port, debug);
        sender.SendFile(filepath);
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
