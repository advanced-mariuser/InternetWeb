#include "Resolver.h"

#include <cstring>

#include "../Common/Socket.h"
#include <iostream>
#include <arpa/inet.h>

Resolver::Resolver(bool debug_mode) : m_debug(debug_mode)
{
    m_root_servers = {"198.41.0.4", "199.9.14.201", "192.33.4.12", "199.7.91.13"};
}

std::vector<char> Resolver::PerformQuery(const std::string& serverIp, const std::vector<char>& query)
{
    UdpSocket udpSocket;
    if (m_debug)
    {
        std::cout << "  Sending UDP query to " << serverIp << "...\n";
    }
    udpSocket.SendTo(query, serverIp, 53);
    auto responseData = udpSocket.RecvFrom(512, 5);

    DnsMessage preliminaryParse(responseData);
    if (preliminaryParse.IsTruncated())
    {
        if (m_debug)
        {
            std::cout << "  UDP response truncated. Retrying with TCP...\n";
        }
        TcpSocket tcpSocket;
        tcpSocket.Connect(serverIp, 53);

        uint16_t lenNet = htons(query.size());
        std::vector<char> tcpQuery;
        tcpQuery.insert(tcpQuery.end(), (char*)&lenNet, (char*)&lenNet + sizeof(lenNet));
        tcpQuery.insert(tcpQuery.end(), query.begin(), query.end());

        tcpSocket.Write(tcpQuery);
        return tcpSocket.Read();
    }
    return responseData;
}

std::vector<std::string> Resolver::Resolve(const std::string& domain, uint16_t recordType)
{
    std::vector<std::string> currentNameservers = m_root_servers;

    for (int i = 0; i < 30; ++i)
    {
        DnsMessage response = QueryServersForResponse(currentNameservers, domain, recordType);

        ResolutionStepResult result = ProcessDnsResponse(response, recordType);

        if (!result.finalAnswers.empty())
        {
            return result.finalAnswers;
        }

        if (!result.nextNameservers.empty())
        {
            currentNameservers = result.nextNameservers;
        }
        else
        {
            throw std::runtime_error("Resolution failed: No answer and no further nameservers provided");
        }
    }
    throw std::runtime_error("Resolution failed: Max iterations reached");
}

DnsMessage Resolver::QueryServersForResponse(const std::vector<std::string>& nameservers, const std::string& domain,
                                             uint16_t recordType)
{
    if (m_debug)
    {
        std::cout << "\n--- Querying nameservers: ";
        for (const auto& ns : nameservers) std::cout << ns << " ";
        std::cout << "\n";
    }

    std::vector<char> query = DnsMessage::BuildQuery(domain, recordType);
    std::vector<char> responseData;

    for (const auto& ns_ip : nameservers)
    {
        try
        {
            responseData = PerformQuery(ns_ip, query);
            break;
        }
        catch (const std::exception& e)
        {
            if (m_debug) std::cerr << "  Query to " << ns_ip << " failed: " << e.what() << "\n";
        }
    }

    if (responseData.empty()) throw std::runtime_error("All nameservers failed to respond in this step");

    return DnsMessage(responseData);
}

Resolver::ResolutionStepResult Resolver::ProcessDnsResponse(const DnsMessage& response, uint16_t recordType)
{
    if (response.GetResponseCode() == 3)
    {
        throw std::runtime_error("Domain not found (NXDOMAIN)");
    }
    if (response.GetResponseCode() != 0)
    {
        throw std::runtime_error(
            "DNS server returned an error (RCODE=" + std::to_string(response.GetResponseCode()) + ")");
    }

    std::vector<std::string> finalAnswers = ExtractAnswers(response, recordType);
    if (!finalAnswers.empty())
    {
        return {finalAnswers, {}};
    }

    std::vector<std::string> nextNameservers = HandleReferral(response);
    return {{}, nextNameservers};
}

std::vector<std::string> Resolver::ExtractAnswers(const DnsMessage& response, uint16_t record_type)
{
    std::vector<std::string> results;
    for (const auto& answer : response.GetAnswers())
    {
        if (answer.type == record_type)
        {
            if (record_type == 1)
            {
                // A
                in_addr addr;
                memcpy(&addr, answer.rdata.data(), sizeof(addr));
                results.push_back(inet_ntoa(addr));
            }
            else if (record_type == 28)
            {
                // AAAA
                char ipStr[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, answer.rdata.data(), ipStr, sizeof(ipStr));
                results.push_back(ipStr);
            }
        }
    }
    return results;
}

std::vector<std::string> Resolver::HandleReferral(const DnsMessage& response)
{
    bool isReferral = !response.GetAuthorities().empty() && response.GetAuthorities()[0].type == 2; // NS record

    if (!isReferral)
    {
        return {};
    }

    std::vector<std::string> nextLevelNsIps;
    for (const auto& addRecord : response.GetAdditionals())
    {
        if (addRecord.type == 1)
        {
            // A record
            in_addr addr;
            memcpy(&addr, addRecord.rdata.data(), sizeof(addr));
            nextLevelNsIps.push_back(inet_ntoa(addr));
        }
    }

    if (nextLevelNsIps.empty())
    {
        std::string nsDomain = response.GetAuthorities()[0].rdata;
        if (m_debug)
        {
            std::cout << "  No glue record found. Recursively resolving NS domain: " << nsDomain << "\n";
        }
        return Resolve(nsDomain, 1);
    }

    return nextLevelNsIps;
}
