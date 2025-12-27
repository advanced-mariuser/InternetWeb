#include "src/Core/RdtReceiver.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <listening_port> <output_filepath> [-d]\n";
        return EXIT_FAILURE;
    }
    try
    {
        uint16_t port = std::stoi(argv[1]);
        std::string filepath = argv[2];
        bool debug = (argc == 4 && std::string(argv[3]) == "-d");

        RdtReceiver receiver(port, filepath, debug);
        receiver.ReceiveFile();
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
