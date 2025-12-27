#include "SmtpClient.h"
#include "../Mime/MimeBuilder.h"
#include "../Common/Base64.h"
#include <iostream>
#include <vector>

SmtpClient::SmtpClient(std::string host, uint16_t port)
    : m_host(std::move(host)), m_port(port)
{
}

void SmtpClient::Send(const Email& email, const std::string& heloDomain, const std::string& user,
                      const std::string& pass)
{
    m_socket.Create();
    std::cout << "[Info] Connecting to " << m_host << ":" << m_port << "...\n";
    m_socket.Connect(m_host, m_port);
    ReadResponse();

    SendCommand("EHLO " + heloDomain + "\r\n", 250);
    PerformTlsHandshake();

    SendCommand("EHLO " + heloDomain + "\r\n", 250);
    PerformAuth(user, pass);

    SendCommand("MAIL FROM: <" + email.from + ">\r\n", 250);
    SendCommand("RCPT TO: <" + email.to + ">\r\n", 250);
    SendCommand("DATA\r\n", 354);

    MimeBuilder mimeBuilder(email);
    std::string emailData = mimeBuilder.Build();
    m_socket.Write(emailData);

    SendCommand("\r\n.\r\n", 250);
    SendCommand("QUIT\r\n", 221);
}

void SmtpClient::PerformTlsHandshake()
{
    std::cout << "C: STARTTLS\r\n";
    m_socket.Write("STARTTLS\r\n");
    std::string response = ReadResponse();
    if (std::stoi(response.substr(0, 3)) != 220)
    {
        throw std::runtime_error("STARTTLS command failed: " + response);
    }
    std::cout << "[Info] Upgrading to TLS...\n";
    m_socket.UpgradeToTls();
    std::cout << "[Info] TLS connection established.\n";
}

void SmtpClient::PerformAuth(const std::string& user, const std::string& pass)
{
    SendCommand("AUTH LOGIN\r\n", 334);
    SendCommand(Base64::Encode(user) + "\r\n", 334);
    SendCommand(Base64::Encode(pass) + "\r\n", 235);
    std::cout << "[Info] Authentication successful.\n";
}

void SmtpClient::SendCommand(const std::string& command, int expectedCode)
{
    std::cout << "C: " << command;
    m_socket.Write(command);
    std::string response = ReadResponse();
    if (std::stoi(response.substr(0, 3)) != expectedCode)
    {
        throw std::runtime_error(
            "Unexpected SMTP response. Expected " + std::to_string(expectedCode) + ", got: " + response);
    }
}

std::string SmtpClient::ReadResponse()
{
    std::vector<char> buffer(2048);
    ssize_t bytesRead = m_socket.Read(buffer.data(), buffer.size() - 1);
    if (bytesRead == 0) throw std::runtime_error("Server closed connection");
    buffer[bytesRead] = '\0';
    std::string response(buffer.data());
    std::cout << "S: " << response;
    // SMTP-ответы могут быть многострочными, здесь для простоты читаем только первую часть
    return response;
}
