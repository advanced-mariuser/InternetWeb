#pragma once
#include <string>

class Socket
{
public:
    Socket();
    ~Socket();
    Socket(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(const Socket&) = delete;
    Socket& operator=(Socket&& other) noexcept;

    void Create();
    void Bind(uint16_t port);
    void Listen(int backlog = 10);
    Socket Accept();
    ssize_t Recv(void* buffer, size_t length);
    ssize_t Send(const void* buffer, size_t length);
    void Close();
    bool IsValid() const;

private:
    explicit Socket(int fd);
    int m_fd;
};
