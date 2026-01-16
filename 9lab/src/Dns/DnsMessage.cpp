#include "DnsMessage.h"
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>
#include <random>

void DomainToDnsFormat(const std::string& domainName, std::vector<char>& outBuffer)
{
    size_t lastDot = 0;
    for (size_t i = 0; i < domainName.length(); ++i)
    {
        if (domainName[i] == '.')
        {
            outBuffer.push_back(i - lastDot);
            for (size_t j = lastDot; j < i; ++j)
            {
                outBuffer.push_back(domainName[j]);
            }
            lastDot = i + 1;
        }
    }
    outBuffer.push_back(domainName.length() - lastDot);
    for (size_t j = lastDot; j < domainName.length(); ++j)
    {
        outBuffer.push_back(domainName[j]);
    }
    outBuffer.push_back(0);
}

std::vector<char> DnsMessage::BuildQuery(const std::string& domainName, uint16_t recordType)
{
    std::vector<char> buffer;
    DnsHeader header{};
    std::random_device rd;
    header.id = htons(rd());
    header.flags = htons(0);
    header.qdcount = htons(1);

    buffer.insert(buffer.end(), (char*)&header, (char*)&header + sizeof(header));
    DomainToDnsFormat(domainName, buffer);

    uint16_t typeNet = htons(recordType);
    uint16_t classNet = htons(1);
    buffer.insert(buffer.end(), (char*)&typeNet, (char*)&typeNet + sizeof(typeNet));
    buffer.insert(buffer.end(), (char*)&classNet, (char*)&classNet + sizeof(classNet));

    return buffer;
}

DnsMessage::DnsMessage(const std::vector<char>& buffer)
{
    if (buffer.size() < sizeof(DnsHeader))
    {
        throw std::runtime_error("DNS response is too small");
    }

    memcpy(&m_header, buffer.data(), sizeof(DnsHeader));
    m_header.id = ntohs(m_header.id);
    m_header.flags = ntohs(m_header.flags);
    m_header.qdcount = ntohs(m_header.qdcount);
    m_header.ancount = ntohs(m_header.ancount);
    m_header.nscount = ntohs(m_header.nscount);
    m_header.arcount = ntohs(m_header.arcount);

    const char* ptr = buffer.data() + sizeof(DnsHeader);
    const char* bufferStart = buffer.data();

    for (int i = 0; i < m_header.qdcount; ++i)
    {
        ReadDnsName(ptr, bufferStart);
        ptr += 4;
    }

    auto parseRecords = [&](std::vector<ResourceRecordData>& records, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            ResourceRecordData rr;
            rr.name = ReadDnsName(ptr, bufferStart);

#pragma pack(push, 1)
            struct RrFields
            {
                uint16_t type, r_class;
                uint32_t ttl;
                uint16_t rdlength;
            };
#pragma pack(pop)

            RrFields fields;
            memcpy(&fields, ptr, sizeof(fields));
            ptr += sizeof(fields);

            rr.type = ntohs(fields.type);
            rr.r_class = ntohs(fields.r_class);
            rr.ttl = ntohl(fields.ttl);
            rr.rdlength = ntohs(fields.rdlength);

            if (rr.type == 2)
            {
                const char* rdataPtr = ptr;
                rr.rdata = ReadDnsName(rdataPtr, bufferStart);
            }
            else
            {
                rr.rdata.assign(ptr, rr.rdlength);
            }
            ptr += rr.rdlength;
            records.push_back(rr);
        }
    };

    parseRecords(m_answers, m_header.ancount);
    parseRecords(m_authorities, m_header.nscount);
    parseRecords(m_additionals, m_header.arcount);
}

std::string DnsMessage::ReadDnsName(const char*& ptr, const char* bufferStart)
{
    std::string name;
    const char* originalPtr = ptr;
    bool jumped = false;

    while (*ptr != 0)
    {
        if ((*ptr & 0xC0) == 0xC0)
        {
            if (!jumped)
            {
                originalPtr = ptr + 2;
                jumped = true;
            }
            uint16_t offset = ntohs(*(uint16_t*)ptr) & 0x3FFF;
            ptr = bufferStart + offset;
        }
        else
        {
            uint8_t len = *ptr++;
            name.append(ptr, len);
            ptr += len;
            if (*ptr != 0) name += ".";
        }
    }

    if (!jumped)
    {
        ptr++;
    }
    else
    {
        ptr = originalPtr;
    }

    return name;
}

DnsHeader DnsMessage::GetHeader() const { return m_header; }
bool DnsMessage::IsTruncated() const { return (m_header.flags >> 9) & 1; }
uint8_t DnsMessage::GetResponseCode() const { return m_header.flags & 0xF; }
std::vector<ResourceRecordData> DnsMessage::GetAnswers() const { return m_answers; }
std::vector<ResourceRecordData> DnsMessage::GetAuthorities() const { return m_authorities; }
std::vector<ResourceRecordData> DnsMessage::GetAdditionals() const { return m_additionals; }
