#include "Socket.h"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/err.h>

static std::string GetSocketErrorString() { return "Socket error: " + std::string(strerror(errno)); }

Socket::Socket()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    m_ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!m_ssl_ctx) throw std::runtime_error("Failed to create SSL_CTX");
}

Socket::~Socket() { Close(); }

Socket::Socket(Socket&& other) noexcept
    : m_fd(std::exchange(other.m_fd, -1)),
      m_ssl_ctx(std::exchange(other.m_ssl_ctx, nullptr)),
      m_ssl(std::exchange(other.m_ssl, nullptr))
{
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other)
    {
        Close();
        m_fd = std::exchange(other.m_fd, -1);
        m_ssl_ctx = std::exchange(other.m_ssl_ctx, nullptr);
        m_ssl = std::exchange(other.m_ssl, nullptr);
    }
    return *this;
}

void Socket::Create()
{
    if (m_fd != -1) throw std::logic_error("Socket already created");
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) throw std::runtime_error("socket() failed: " + GetSocketErrorString());
}

void Socket::Connect(const std::string& host, uint16_t port)
{
    addrinfo hints = {}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result) != 0)
    {
        throw std::runtime_error("getaddrinfo() failed for host " + host);
    }
    if (connect(m_fd, result->ai_addr, result->ai_addrlen) == -1)
    {
        freeaddrinfo(result);
        throw std::runtime_error("connect() failed: " + GetSocketErrorString());
    }
    freeaddrinfo(result);
}

void Socket::UpgradeToTls()
{
    m_ssl = SSL_new(m_ssl_ctx);
    if (!m_ssl) throw std::runtime_error("SSL_new failed");
    SSL_set_fd(m_ssl, m_fd);
    if (SSL_connect(m_ssl) <= 0)
    {
        throw std::runtime_error("SSL_connect failed: " + SslReadError());
    }
}

ssize_t Socket::Read(void* buffer, size_t length)
{
    if (m_ssl)
    {
        int bytes = SSL_read(m_ssl, buffer, length);
        if (bytes < 0) throw std::runtime_error("SSL_read failed: " + SslReadError());
        return bytes;
    }
    ssize_t bytes = read(m_fd, buffer, length);
    if (bytes < 0) throw std::runtime_error("read() failed: " + GetSocketErrorString());
    return bytes;
}

void Socket::Write(const std::string& data)
{
    if (m_ssl)
    {
        int bytes = SSL_write(m_ssl, data.c_str(), data.length());
        if (bytes <= 0) throw std::runtime_error("SSL_write failed: " + SslReadError());
    }
    else
    {
        ssize_t bytes = write(m_fd, data.c_str(), data.length());
        if (bytes < 0) throw std::runtime_error("write() failed: " + GetSocketErrorString());
        if (static_cast<size_t>(bytes) != data.length()) throw std::runtime_error("Incomplete write");
    }
}

void Socket::Close()
{
    if (m_ssl)
    {
        SSL_shutdown(m_ssl);
        SSL_free(m_ssl);
        m_ssl = nullptr;
    }
    if (m_ssl_ctx)
    {
        SSL_CTX_free(m_ssl_ctx);
        m_ssl_ctx = nullptr;
    }
    if (m_fd != -1)
    {
        close(m_fd);
        m_fd = -1;
    }
}

std::string Socket::SslReadError()
{
    unsigned long errorCode = ERR_get_error();
    char errorBuf[256];
    ERR_error_string_n(errorCode, errorBuf, sizeof(errorBuf));
    return std::string(errorBuf);
}
