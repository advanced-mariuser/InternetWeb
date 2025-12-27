#pragma once
#include <string>
#include <vector>

class TcpSocket
{
public:
    TcpSocket();
    ~TcpSocket();
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& other) noexcept;
    TcpSocket& operator=(TcpSocket&& other) noexcept;

    void Create();
    void Connect(const std::string& host, uint16_t port);
    void Bind(uint16_t port);
    void Listen(int backlog = 20);
    TcpSocket Accept();

    ssize_t Recv(void* buffer, size_t length);
    ssize_t Send(const void* buffer, size_t length);

    void Close();
    bool IsValid() const;
    int GetFd() const;

private:
    explicit TcpSocket(int fd);
    int m_fd;
};
