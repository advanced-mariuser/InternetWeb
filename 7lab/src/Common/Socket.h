#pragma once
#include <string>
#include <openssl/ssl.h>

class Socket
{
public:
    Socket();
    ~Socket();
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void Create();
    void Connect(const std::string& host, uint16_t port);
    void UpgradeToTls();
    ssize_t Read(void* buffer, size_t length);
    void Write(const std::string& data);
    void Close();

private:
    std::string SslReadError();

    int m_fd = -1;
    SSL_CTX* m_ssl_ctx = nullptr;
    SSL* m_ssl = nullptr;
};
