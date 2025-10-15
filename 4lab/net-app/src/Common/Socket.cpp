#include "Socket.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

static std::string GetSocketErrorString()
{
    return "Socket error " + std::to_string(errno) + ": " + strerror(errno);
}

Socket::Socket() : m_fd(-1) {}

Socket::Socket(int fd) : m_fd(fd) {}

Socket::~Socket()
{
    Close();
}

Socket::Socket(Socket&& other) noexcept
        : m_fd(std::exchange(other.m_fd, -1))
{
}

Socket& Socket::operator=(Socket&& other) noexcept
{
    if (this != &other)
    {
        Close();
        m_fd = std::exchange(other.m_fd, -1);
    }
    return *this;
}

bool Socket::IsValid() const
{
    return m_fd != -1;
}

void Socket::Create()
{
    if (IsValid())
    {
        throw std::logic_error("Socket is already created.");
    }
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!IsValid())
    {
        throw std::runtime_error("socket() failed: " + GetSocketErrorString());
    }
}

void Socket::Connect(const std::string& host, uint16_t port)
{
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
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

void Socket::Bind(uint16_t port)
{
    int reuse = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed: " + GetSocketErrorString());
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(m_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1)
    {
        throw std::runtime_error("bind() failed: " + GetSocketErrorString());
    }
}

void Socket::Listen(int backlog)
{
    if (listen(m_fd, backlog) == -1)
    {
        throw std::runtime_error("listen() failed: " + GetSocketErrorString());
    }
}

Socket Socket::Accept()
{
    int client_fd = accept(m_fd, nullptr, nullptr);
    if (client_fd == -1)
    {
        if (errno == EBADF || errno == EINVAL)
        {
            return Socket(-1);
        }
        throw std::runtime_error("accept() failed: " + GetSocketErrorString());
    }
    return Socket(client_fd);
}

void Socket::SendAll(const void* data, size_t size)
{
    const char* buffer = static_cast<const char*>(data);
    size_t total_sent = 0;
    while (total_sent < size)
    {
        ssize_t sent = send(m_fd, buffer + total_sent, size - total_sent, 0);
        if (sent == -1)
        {
            throw std::runtime_error("send() failed: " + GetSocketErrorString());
        }
        total_sent += sent;
    }
}

void Socket::RecvAll(void* data, size_t size)
{
    char* buffer = static_cast<char*>(data);
    size_t total_received = 0;
    while (total_received < size)
    {
        ssize_t received = recv(m_fd, buffer + total_received, size - total_received, 0);
        if (received == -1)
        {
            throw std::runtime_error("recv() failed: " + GetSocketErrorString());
        }
        if (received == 0)
        {
            throw std::runtime_error("Connection closed by peer");
        }
        total_received += received;
    }
}

void Socket::Close()
{
    if (IsValid())
    {
        close(m_fd);
        m_fd = -1;
    }
}