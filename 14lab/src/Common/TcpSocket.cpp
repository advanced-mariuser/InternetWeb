#include "TcpSocket.h"
#include <stdexcept>
#include <cstring>
#include <utility>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

static std::string GetSocketErrorString() { return "Socket error: " + std::string(strerror(errno)); }

TcpSocket::TcpSocket() : m_fd(-1)
{
}

TcpSocket::TcpSocket(int fd) : m_fd(fd)
{
}

TcpSocket::~TcpSocket() { Close(); }

TcpSocket::TcpSocket(TcpSocket&& other) noexcept : m_fd(std::exchange(other.m_fd, -1))
{
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept
{
    if (this != &other)
    {
        Close();
        m_fd = std::exchange(other.m_fd, -1);
    }
    return *this;
}

bool TcpSocket::IsValid() const { return m_fd != -1; }
int TcpSocket::GetFd() const { return m_fd; }

void TcpSocket::Create()
{
    if (IsValid())
    {
        throw std::logic_error("Socket already created");
    }
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!IsValid())
    {
        throw std::runtime_error("socket() failed: " + GetSocketErrorString());
    }
}

void TcpSocket::Connect(const std::string& host, uint16_t port)
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
        throw std::runtime_error("connect() to " + host + " failed: " + GetSocketErrorString());
    }
    freeaddrinfo(result);
}

void TcpSocket::Bind(uint16_t port)
{
    int reuse = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("bind() failed: " + GetSocketErrorString());
    }
}

void TcpSocket::Listen(int backlog)
{
    if (listen(m_fd, backlog) == -1)
    {
        throw std::runtime_error("listen() failed");
    }
}

TcpSocket TcpSocket::Accept()
{
    int clientFd = accept(m_fd, nullptr, nullptr);
    if (clientFd < 0)
    {
        if (errno == EBADF || errno == EINVAL)
        {
            return TcpSocket(-1);
        }
        throw std::runtime_error("accept() failed: " + GetSocketErrorString());
    }
    return TcpSocket(clientFd);
}

ssize_t TcpSocket::Recv(void* buffer, size_t length)
{
    ssize_t bytes = recv(m_fd, buffer, length, 0);
    if (bytes < 0)
    {
        throw std::runtime_error("recv() failed: " + GetSocketErrorString());
    }
    return bytes;
}

ssize_t TcpSocket::Send(const void* buffer, size_t length)
{
    ssize_t totalSent = 0;
    while (totalSent < length)
    {
        ssize_t sent = send(m_fd, static_cast<const char*>(buffer) + totalSent, length - totalSent, 0);
        if (sent < 0)
        {
            throw std::runtime_error("send() failed: " + GetSocketErrorString());
        }
        totalSent += sent;
    }
    return totalSent;
}

void TcpSocket::Close()
{
    if (IsValid())
    {
        close(m_fd);
        m_fd = -1;
    }
}
