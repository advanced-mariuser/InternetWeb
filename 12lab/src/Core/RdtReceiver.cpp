#include "RdtReceiver.h"
#include "../Protocol/Packet.h"
#include <iostream>
#include <fstream>
#include <arpa/inet.h>

RdtReceiver::RdtReceiver(uint16_t port, std::string filepath, bool debug)
    : m_port(port), m_filepath(std::move(filepath)), m_debug(debug), m_channel(0.1)
{
    m_channel.GetSocket().Bind(m_port);
}

void RdtReceiver::ReceiveFile()
{
    std::ofstream file(m_filepath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open output file: " + m_filepath);
    }

    std::cout << "Receiver listening on port " << m_port << "...\n";

    while (true)
    {
        sockaddr_in srcAddr;
        auto data = m_channel.RecvFrom(srcAddr);
        if (data.empty())
        {
            continue;
        }

        Packet p;
        p.deserialize(data);

        if (!p.isChecksumValid())
        {
            if (m_debug)
            {
                std::cout << "Received corrupted packet. Discarding.\n";
            }
            continue;
        }

        if (p.flags & FLAG_FIN)
        {
            if (m_debug)
            {
                std::cout << "Received FIN packet. Shutting down.\n";
            }
            break;
        }

        if (p.seqNum == m_expectedSeqNum)
        {
            if (m_debug)
            {
                std::cout << "Received packet " << p.seqNum << ". Accepting.\n";
            }
            file.write(p.payload, p.length);

            Packet ackP;
            ackP.ackNum = m_expectedSeqNum;
            ackP.flags = FLAG_ACK;

            std::string srcIp = inet_ntoa(srcAddr.sin_addr);
            uint16_t srcPort = ntohs(srcAddr.sin_port);
            m_channel.SendTo(ackP.serialize(), srcIp, srcPort);
            if (m_debug)
            {
                std::cout << "Sent ACK " << ackP.ackNum << "\n";
            }

            m_expectedSeqNum++;
        }
        else
        {
            if (m_debug)
            {
                std::cout << "Received out-of-order packet " << p.seqNum
                    << " (expected " << m_expectedSeqNum << "). Discarding.\n";
            }

            if (m_expectedSeqNum > 0)
            {
                Packet ackP;
                ackP.ackNum = m_expectedSeqNum - 1;
                ackP.flags = FLAG_ACK;
                std::string srcIp = inet_ntoa(srcAddr.sin_addr);
                uint16_t srcPort = ntohs(srcAddr.sin_port);
                m_channel.SendTo(ackP.serialize(), srcIp, srcPort);
                if (m_debug)
                {
                    std::cout << "Resent ACK " << ackP.ackNum << "\n";
                }
            }
        }
    }
    std::cout << "\nFile received successfully and saved to " << m_filepath << "\n";
}
