#pragma once
#include <cstdint>
#include <string>
#include <vector>

#pragma pack(push, 1)
struct DnsHeader
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

struct ResourceRecordData
{
    std::string name;
    uint16_t type;
    uint16_t r_class;
    uint32_t ttl;
    uint16_t rdlength;
    std::string rdata;
};
#pragma pack(pop)

class DnsMessage
{
public:
    DnsMessage() = default;
    explicit DnsMessage(const std::vector<char>& buffer);

    static std::vector<char> BuildQuery(const std::string& domainName, uint16_t recordType);

    DnsHeader GetHeader() const;
    bool IsTruncated() const;
    uint8_t GetResponseCode() const;
    std::vector<ResourceRecordData> GetAnswers() const;
    std::vector<ResourceRecordData> GetAuthorities() const;
    std::vector<ResourceRecordData> GetAdditionals() const;

private:
    std::string ReadDnsName(const char*& ptr, const char* bufferStart);

    DnsHeader m_header;
    std::vector<ResourceRecordData> m_answers;
    std::vector<ResourceRecordData> m_authorities;
    std::vector<ResourceRecordData> m_additionals;
};
