#include "HttpRequest.h"
#include <sstream>
#include <stdexcept>

bool HttpRequest::Parse(const std::vector<char>& rawRequest)
{
    std::string requestStr(rawRequest.begin(), rawRequest.end());
    std::istringstream requestStream(requestStr);
    std::string line;

    if (!std::getline(requestStream, line) || line.empty())
    {
        return false;
    }
    std::istringstream lineStream(line);
    lineStream >> m_method >> m_url >> m_version;

    if (m_method != "GET")
    {
        return false;
    }
    ParseUrl();

    while (std::getline(requestStream, line) && !line.empty() && line != "\r")
    {
        auto colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string name = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);
            if (value.back() == '\r')
            {
                value.pop_back();
            }
            m_headers[name] = value;
        }
    }
    if (m_headers.count("Host"))
    {
        m_host = m_headers["Host"];
    }
    return true;
}

void HttpRequest::ParseUrl()
{
    if (m_url.rfind("http://", 0) != 0)
    {
        throw std::runtime_error("Proxy supports only absolute HTTP URLs");
    }
    std::string urlWithoutHttp = m_url.substr(7);
    auto pathPos = urlWithoutHttp.find('/');
    if (pathPos == std::string::npos)
    {
        m_host = urlWithoutHttp;
        m_path = "/";
    }
    else
    {
        m_host = urlWithoutHttp.substr(0, pathPos);
        m_path = urlWithoutHttp.substr(pathPos);
    }
    auto colonPos = m_host.find(':');
    if (colonPos != std::string::npos)
    {
        m_port = std::stoi(m_host.substr(colonPos + 1));
        m_host = m_host.substr(0, colonPos);
    }
}

const std::string& HttpRequest::GetMethod() const { return m_method; }
const std::string& HttpRequest::GetUrl() const { return m_url; }
const std::string& HttpRequest::GetHost() const { return m_host; }
uint16_t HttpRequest::GetPort() const { return m_port; }

std::string HttpRequest::BuildRequestForTargetServer() const
{
    std::ostringstream oss;
    oss << m_method << " " << m_path << " " << "HTTP/1.0\r\n";\
    oss << "Host: " << m_host << "\r\n";
    oss << "Connection: close\r\n";
    for (const auto& [name, value] : m_headers)
    {
        if (name != "Host" && name != "Proxy-Connection")
        {
            oss << name << ": " << value << "\r\n";
        }
    }
    oss << "\r\n";
    return oss.str();
}
