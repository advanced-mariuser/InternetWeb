#include "UdpSocket.h"
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static std::string GetSocketErrorString() { return "Socket error: " + std::string(strerror(errno)); }

UdpSocket::UdpSocket()
{
    m_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_fd < 0)
    {
        throw std::runtime_error("UDP socket() failed: " + GetSocketErrorString());
    }
}

UdpSocket::~UdpSocket()
{
    if (m_fd >= 0)
    {
        close(m_fd);
    }
}

void UdpSocket::Bind(uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        throw std::runtime_error("bind() failed: " + GetSocketErrorString());
    }
}

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

std::vector<char> UdpSocket::RecvFrom(sockaddr_in& sourceAddr)
{
    std::vector<char> buffer(2048);
    socklen_t addrLen = sizeof(sourceAddr);
    ssize_t bytesRead = recvfrom(m_fd, buffer.data(), buffer.size(), 0, (struct sockaddr*)&sourceAddr, &addrLen);
    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw std::runtime_error("recvfrom() failed: " + GetSocketErrorString());
    }
    buffer.resize(bytesRead);
    return buffer;
}
