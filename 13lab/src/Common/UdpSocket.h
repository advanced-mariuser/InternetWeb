#pragma once
#include <string>
#include <vector>
#include <optional>
#include <netinet/in.h>

class UdpSocket {
public:
    UdpSocket();
    ~UdpSocket();
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;
    
    void Bind(uint16_t port);
    void SetRecvTimeout(int seconds, int microseconds);
    void SendTo(const std::string& data, const std::string& ip, uint16_t port);
    std::optional<std::string> RecvFrom(sockaddr_in& sourceAddr);

private:
    int m_fd = -1;
};