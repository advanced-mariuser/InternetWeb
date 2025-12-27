#pragma once
#include <cstdint>
#include <vector>
#include <cstring>
#include <arpa/inet.h>

constexpr size_t PAYLOAD_SIZE = 1024;
constexpr size_t PACKET_HEADER_SIZE = 12;

enum PacketFlags : uint8_t
{
    FLAG_NONE = 0,
    FLAG_ACK = 1 << 0,
    FLAG_FIN = 1 << 1,
};

#pragma pack(push, 1)
struct Packet
{
    uint32_t seqNum = 0;
    uint32_t ackNum = 0;
    uint8_t flags = FLAG_NONE;
    uint16_t checksum = 0;
    uint16_t length = 0;
    char payload[PAYLOAD_SIZE];

private:
    uint16_t calculateSum(const char* data, size_t size) const
    {
        uint32_t sum = 0;
        const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data);

        for (size_t i = 0; i < size / 2; ++i)
        {
            sum += ntohs(ptr[i]);
        }
        if (size % 2 == 1)
        {
            sum += data[size - 1];
        }
        while (sum >> 16)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        return static_cast<uint16_t>(sum);
    }

public:
    bool isChecksumValid() const
    {
        Packet copy = *this;
        uint16_t received_checksum = copy.checksum;
        copy.checksum = 0;

        copy.convertToNetworkOrder();

        const char* data_ptr = reinterpret_cast<const char*>(&copy);
        size_t total_size = PACKET_HEADER_SIZE + this->length;

        uint16_t sum = calculateSum(data_ptr, total_size);
        return (uint16_t)(~sum) == received_checksum;
    }

    void convertToHostOrder()
    {
        seqNum = ntohl(seqNum);
        ackNum = ntohl(ackNum);
        length = ntohs(length);
    }

    void convertToNetworkOrder()
    {
        seqNum = htonl(seqNum);
        ackNum = htonl(ackNum);
        length = htons(length);
    }

    std::vector<char> serialize() const
    {
        Packet copy = *this;
        copy.convertToNetworkOrder();

        copy.checksum = 0;
        const char* data_ptr = reinterpret_cast<const char*>(&copy);
        size_t total_size = PACKET_HEADER_SIZE + this->length;

        uint16_t sum = calculateSum(data_ptr, total_size);
        copy.checksum = ~sum;

        const char* start = reinterpret_cast<const char*>(&copy);
        return {start, start + total_size};
    }

    void deserialize(const std::vector<char>& data)
    {
        if (data.size() > sizeof(Packet) || data.size() < PACKET_HEADER_SIZE) return;
        std::memcpy(this, data.data(), data.size());

        this->convertToHostOrder();
    }
};
#pragma pack(pop)
