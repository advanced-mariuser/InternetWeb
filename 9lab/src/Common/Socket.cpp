#include "Socket.h"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

static std::string GetSocketErrorString() { return "Socket error: " + std::string(strerror(errno)); }

UdpSocket::UdpSocket()
{
    m_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_fd < 0)
    {
        throw std::runtime_error("UDP socket() failed: " + GetSocketErrorString());
    }
}

UdpSocket::~UdpSocket() { if (m_fd >= 0) close(m_fd); }

void UdpSocket::SendTo(const std::vector<char>& data, const std::string& ip, uint16_t port)
{
    sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &destAddr.sin_addr);

    if (sendto(m_fd, data.data(), data.size(), 0, (struct sockaddr*)&destAddr, sizeof(destAddr)) < 0)
    {
        throw std::runtime_error("sendto() failed: " + GetSocketErrorString());
    }
}

std::vector<char> UdpSocket::RecvFrom(size_t maxSize, int timeoutSec)
{
    struct timeval tv;
    tv.tv_sec = timeoutSec;
    tv.tv_usec = 0;
    if (setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        throw std::runtime_error("setsockopt(SO_RCVTIMEO) failed: " + GetSocketErrorString());
    }

    std::vector<char> buffer(maxSize);
    ssize_t bytesRead = recvfrom(m_fd, buffer.data(), buffer.size(), 0, nullptr, nullptr);
    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            throw std::runtime_error("recvfrom() timed out");
        }
        throw std::runtime_error("recvfrom() failed: " + GetSocketErrorString());
    }
    buffer.resize(bytesRead);
    return buffer;
}

TcpSocket::TcpSocket()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0)
    {
        throw std::runtime_error("TCP socket() failed: " + GetSocketErrorString());
    }
}

TcpSocket::~TcpSocket() { if (m_fd >= 0) close(m_fd); }

void TcpSocket::Connect(const std::string& ip, uint16_t port)
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);
    if (connect(m_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("connect() failed for TCP: " + GetSocketErrorString());
    }
}

void TcpSocket::Write(const std::vector<char>& data)
{
    if (write(m_fd, data.data(), data.size()) != (ssize_t)data.size())
    {
        throw std::runtime_error("Incomplete TCP write");
    }
}

std::vector<char> TcpSocket::Read()
{
    uint16_t lenNet;
    if (read(m_fd, &lenNet, sizeof(lenNet)) != sizeof(lenNet))
    {
        throw std::runtime_error("Failed to read DNS message length from TCP stream");
    }
    uint16_t lenHost = ntohs(lenNet);

    std::vector<char> buffer(lenHost);
    ssize_t totalRead = 0;
    while (totalRead < lenHost)
    {
        ssize_t bytesRead = read(m_fd, buffer.data() + totalRead, lenHost - totalRead);
        if (bytesRead <= 0)
        {
            throw std::runtime_error("Failed to read full DNS message from TCP stream");
        }
        totalRead += bytesRead;
    }
    return buffer;
}
