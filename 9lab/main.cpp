#include "src/Core/Resolver.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > 4)
    {
        std::cerr << "Usage: " << argv[0] << " <domain_name> <record_type> [-d]\n";
        std::cerr << "Example: " << argv[0] << " google.com A\n";
        std::cerr << "Example (debug): " << argv[0] << " yandex.ru AAAA -d\n";
        return EXIT_FAILURE;
    }

    try
    {
        std::string domain = argv[1];
        std::string typeStr = argv[2];
        bool debugMode = (argc == 4 && std::string(argv[3]) == "-d");

        uint16_t recordType;
        if (typeStr == "A")
        {
            recordType = 1;
        }
        else if (typeStr == "AAAA")
        {
            recordType = 28;
        }
        else
        {
            throw std::runtime_error("Unsupported record type: " + typeStr);
        }

        Resolver resolver(debugMode);
        std::vector<std::string> results = resolver.Resolve(domain, recordType);

        std::cout << "\nResolution result for " << domain << " (" << typeStr << "):\n";
        for (const auto& result : results)
        {
            std::cout << "  " << result << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
