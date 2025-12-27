#pragma once
#include <string>
#include <vector>

class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    void SendTo(const std::vector<char>& data, const std::string& ip, uint16_t port);
    std::vector<char> RecvFrom(size_t maxSize, int timeoutSec);

private:
    int m_fd = -1;
};

class TcpSocket
{
public:
    TcpSocket();
    ~TcpSocket();
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    void Connect(const std::string& ip, uint16_t port);
    void Write(const std::vector<char>& data);
    std::vector<char> Read();

private:
    int m_fd = -1;
};
