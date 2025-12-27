#include "src/Core/SmtpClient.h"
#include <iostream>
#include <vector>

void PrintUsage(const char* prog_name)
{
    std::cerr << "Usage: " << prog_name <<
        " <server> <port> <user> <pass> <from> <to> <helo> [attachment1] [attachment2] ...\n";
    std::cerr << "Example (Gmail): " << prog_name
        << " smtp.gmail.com 587 user@gmail.com \"app_pass\" from@gmail.com to@example.com my-pc report.pdf image.jpg\n";
    std::cerr << "\nNOTE: For Gmail, use an 'App Password', not your regular password.\n";
}

int main(int argc, char* argv[])
{
    if (argc < 8)
    {
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }

    try
    {
        std::string server = argv[1];
        uint16_t port = std::stoi(argv[2]);
        std::string user = argv[3];
        std::string pass = argv[4];

        Email email;
        email.from = argv[5];
        email.to = argv[6];
        std::string heloDomain = argv[7];

        email.subject = "Advanced C++ SMTP Client Test";
        email.body = "Hello from a C++ SMTP client!\n\nThis email supports TLS, Authentication, and MIME attachments.";

        for (int i = 8; i < argc; ++i)
        {
            email.attachments.push_back({argv[i]});
        }

        SmtpClient client(server, port);
        client.Send(email, heloDomain, user, pass);

        std::cout << "\n[SUCCESS] Email has been sent successfully!\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
