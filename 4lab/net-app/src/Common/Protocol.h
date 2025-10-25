#pragma once

#include "Socket.h"
#include <cstdint>
#include <string>
#include <arpa/inet.h>

namespace Protocol
{
    struct Message
    {
        std::string name;
        int32_t number;
    };

    inline void WriteMessage(Socket& socket, const Message& msg)
    {
        uint32_t nameLenNet = htonl(static_cast<uint32_t>(msg.name.length()));
        socket.SendAll(&nameLenNet, sizeof(nameLenNet));

        socket.SendAll(msg.name.data(), msg.name.length());

        int32_t numberNet = htonl(msg.number);
        socket.SendAll(&numberNet, sizeof(numberNet));
    }

    inline Message ReadMessage(Socket& socket)
    {
        Message msg;

        uint32_t nameLenNet = 0;
        socket.RecvAll(&nameLenNet, sizeof(nameLenNet));
        uint32_t nameLenHost = ntohl(nameLenNet);

        if (nameLenHost > 0 && nameLenHost < 2048)
        {
            msg.name.resize(nameLenHost);
            socket.RecvAll(msg.name.data(), nameLenHost);
        }

        int32_t numberNet = 0;
        socket.RecvAll(&numberNet, sizeof(numberNet));
        msg.number = ntohl(numberNet);

        return msg;
    }

} // namespace Protocol