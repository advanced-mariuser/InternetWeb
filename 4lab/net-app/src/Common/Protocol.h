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
        uint32_t name_len_net = htonl(static_cast<uint32_t>(msg.name.length()));
        socket.SendAll(&name_len_net, sizeof(name_len_net));

        socket.SendAll(msg.name.data(), msg.name.length());

        int32_t number_net = htonl(msg.number);
        socket.SendAll(&number_net, sizeof(number_net));
    }

    inline Message ReadMessage(Socket& socket)
    {
        Message msg;

        uint32_t name_len_net = 0;
        socket.RecvAll(&name_len_net, sizeof(name_len_net));
        uint32_t name_len_host = ntohl(name_len_net);

        if (name_len_host > 0 && name_len_host < 2048)
        {
            msg.name.resize(name_len_host);
            socket.RecvAll(msg.name.data(), name_len_host);
        }

        int32_t number_net = 0;
        socket.RecvAll(&number_net, sizeof(number_net));
        msg.number = ntohl(number_net);

        return msg;
    }

} // namespace Protocol