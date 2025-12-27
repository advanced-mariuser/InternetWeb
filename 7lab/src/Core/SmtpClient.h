#pragma once
#include "../Common/Socket.h"
#include "../Model/Email.h"
#include <string>

class SmtpClient
{
public:
    SmtpClient(std::string host, uint16_t port);
    void Send(const Email& email, const std::string& heloDomain, const std::string& user, const std::string& pass);

private:
    void SendCommand(const std::string& command, int expectedCode);
    std::string ReadResponse();
    void PerformTlsHandshake();
    void PerformAuth(const std::string& user, const std::string& pass);

    std::string m_host;
    uint16_t m_port;
    Socket m_socket;
};
