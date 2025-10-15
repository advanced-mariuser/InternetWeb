#pragma once

#include <string>

class Socket
{
public:
    Socket();
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void Create();
    void Connect(const std::string& host, uint16_t port);
    void Bind(uint16_t port);
    void Listen(int backlog = 5);
    Socket Accept();

    void SendAll(const void* data, size_t size);
    void RecvAll(void* data, size_t size);

    void Close();
    bool IsValid() const;

private:
    explicit Socket(int fd);

    int m_fd;
};