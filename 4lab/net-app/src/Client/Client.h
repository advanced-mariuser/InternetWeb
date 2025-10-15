#pragma once

#include <string>
#include <cstdint>

class Client
{
public:
    Client(std::string address, uint16_t port);
    void Run();

private:
    std::string m_address;
    uint16_t m_port;
    const std::string m_clientName;
};