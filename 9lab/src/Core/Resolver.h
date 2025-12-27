#pragma once
#include "../Dns/DnsMessage.h"
#include <string>
#include <vector>
#include <optional>

class Resolver
{
public:
    explicit Resolver(bool debug_mode = false);
    std::vector<std::string> Resolve(const std::string& domain, uint16_t recordType);

private:
    struct ResolutionStepResult
    {
        std::vector<std::string> finalAnswers;
        std::vector<std::string> nextNameservers;
    };

    DnsMessage QueryServersForResponse(const std::vector<std::string>& nameservers, const std::string& domain,
                                       uint16_t recordType);
    ResolutionStepResult ProcessDnsResponse(const DnsMessage& response, uint16_t recordType);
    std::vector<std::string> ExtractAnswers(const DnsMessage& response, uint16_t recordType);
    std::vector<std::string> HandleReferral(const DnsMessage& response);

    std::vector<char> PerformQuery(const std::string& serverIp, const std::vector<char>& query);

    bool m_debug;
    std::vector<std::string> m_root_servers;
};
