#pragma once
#include <string>
#include <vector>
#include <netinet/in.h>

class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    void Bind(uint16_t port);
    void SendTo(const std::vector<char>& data, const std::string& ip, uint16_t port);
    std::vector<char> RecvFrom(sockaddr_in& sourceAddr);
    int GetFd() const { return m_fd; }

private:
    int m_fd = -1;
};
