#include "RdtSender.h"
#include <iostream>
#include <fstream>
#include <sys/select.h>
#include <chrono>

RdtSender::RdtSender(std::string host, uint16_t port, bool debug)
    : m_host(std::move(host)), m_port(port), m_debug(debug), m_channel(0.1)
{
}

void RdtSender::SendFile(const std::string& filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    size_t totalBytesSent = 0;

    bool fileFullyRead = false;

    while (!fileFullyRead || m_base < m_nextSeqNum)
    {
        if (!fileFullyRead)
        {
            while (m_nextSeqNum < m_base + m_windowSize && !file.eof())
            {
                Packet p;
                p.seqNum = m_nextSeqNum;
                file.read(p.payload, PAYLOAD_SIZE);
                p.length = file.gcount();

                if (p.length > 0)
                {
                    m_channel.SendTo(p.serialize(), m_host, m_port);
                    if (m_debug)
                    {
                        std::cout << "Sent packet " << p.seqNum << "\n";
                    }
                    m_sentPackets.push_back(p);
                    m_nextSeqNum++;
                    totalBytesSent += p.length;
                }

                if (file.eof())
                {
                    fileFullyRead = true;
                    if (m_debug) std::cout << "[Info] End of file reached.\n";
                    break;
                }
            }
        }

        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(m_channel.GetSocket().GetFd(), &readFds);

        struct timeval tv;
        tv.tv_sec = m_timeout.count() / 1000;
        tv.tv_usec = (m_timeout.count() % 1000) * 1000;

        int ret = select(m_channel.GetSocket().GetFd() + 1, &readFds, nullptr, nullptr, &tv);

        if (ret > 0)
        {
            sockaddr_in srcAddr;
            auto data = m_channel.RecvFrom(srcAddr);
            Packet ackP;
            ackP.deserialize(data);

            if (ackP.isChecksumValid() && (ackP.flags & FLAG_ACK))
            {
                if (m_debug)
                {
                    std::cout << "Received ACK " << ackP.ackNum << "\n";
                }
                m_base = ackP.ackNum + 1;
                m_sentPackets.erase(std::remove_if(m_sentPackets.begin(), m_sentPackets.end(),
                                                   [this](const Packet& p) { return p.seqNum < m_base; }),
                                    m_sentPackets.end());
            }
        }
        else if (ret == 0)
        {
            if (m_debug)
            {
                std::cout << "Timeout! Resending window starting from packet " << m_base << "\n";
            }
            for (auto p : m_sentPackets)
            {
                if (p.seqNum >= m_base)
                {
                    if (m_debug)
                    {
                        std::cout << "  Resending packet " << p.seqNum << "\n";
                    }
                    m_channel.SendTo(p.serialize(), m_host, m_port);
                }
            }
        }
        else
        {
            throw std::runtime_error("select() failed");
        }
    }

    Packet finP;
    finP.seqNum = m_nextSeqNum;
    finP.flags = FLAG_FIN;
    m_channel.SendTo(finP.serialize(), m_host, m_port);
    if (m_debug)
    {
        std::cout << "Sent FIN packet " << finP.seqNum << "\n";
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    std::cout << "\nFile transfer complete.\n";
    std::cout << "Total bytes sent: " << totalBytesSent << "\n";
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    std::cout << "Throughput: " << (totalBytesSent * 8 / 1e6) / elapsed.count() << " Mbps\n";
}
