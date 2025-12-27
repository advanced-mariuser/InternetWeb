#pragma once
#include <string>
#include <map>
#include <vector>

class HttpRequest
{
public:
    bool Parse(const std::vector<char>& rawRequest);

    const std::string& GetMethod() const;
    const std::string& GetUrl() const;
    const std::string& GetHost() const;
    uint16_t GetPort() const;

    std::string BuildRequestForTargetServer() const;

private:
    void ParseUrl();

    std::string m_method;
    std::string m_url;
    std::string m_version;
    std::map<std::string, std::string> m_headers;

    std::string m_host;
    uint16_t m_port = 80;
    std::string m_path;
};
