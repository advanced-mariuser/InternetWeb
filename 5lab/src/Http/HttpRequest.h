#pragma once
#include <string>
#include <vector>
#include <map>

class HttpRequest
{
public:
    enum class Method { GET, UNKNOWN };

    bool Parse(const std::vector<char>& rawRequest);
    Method GetMethod() const;
    const std::string& GetPath() const;

private:
    Method m_method = Method::UNKNOWN;
    std::string m_path;
    std::map<std::string, std::string> m_headers;
};
