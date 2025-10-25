#include "Socket.h"
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

static std::string GetSocketErrorString()
{
    return "Socket error " + std::to_string(errno) + ": " + strerror(errno);
}

Socket::Socket() : m_fd(-1)
{
}

Socket::Socket(int fd) : m_fd(fd)
{
}

Socket::~Socket()
{
    Close();
}

Socket::Socket(Socket&& other) noexcept : m_fd(std::exchange(other.m_fd, -1))
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

void Socket::Bind(uint16_t port)
{
    int reuse = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed: " + GetSocketErrorString());
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(m_fd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
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
    int clientFd = accept(m_fd, nullptr, nullptr);
    if (clientFd == -1)
    {
        if (errno == EBADF || errno == EINVAL) return Socket(-1);
        throw std::runtime_error("accept() failed: " + GetSocketErrorString());
    }
    return Socket(clientFd);
}

ssize_t Socket::Recv(void* buffer, size_t length)
{
    ssize_t bytesReceived = recv(m_fd, buffer, length, 0);
    if (bytesReceived < 0)
    {
        throw std::runtime_error("recv() failed: " + GetSocketErrorString());
    }
    return bytesReceived;
}

ssize_t Socket::Send(const void* buffer, size_t length)
{
    ssize_t bytesSent = send(m_fd, buffer, length, 0);
    if (bytesSent < 0)
    {
        throw std::runtime_error("send() failed: " + GetSocketErrorString());
    }
    return bytesSent;
}

void Socket::Close()
{
    if (IsValid())
    {
        close(m_fd);
        m_fd = -1;
    }
}
