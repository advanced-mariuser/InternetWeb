#include "HttpRequest.h"
#include <sstream>
#include <vector>

bool HttpRequest::Parse(const std::vector<char>& rawRequest)
{
    std::string requestStr(rawRequest.begin(), rawRequest.end());
    std::istringstream requestStream(requestStr);
    std::string line;

    // Parse request line
    if (!std::getline(requestStream, line) || line.empty()) return false;

    std::istringstream lineStream(line);
    std::string methodStr;
    lineStream >> methodStr >> m_path;

    if (methodStr == "GET")
    {
        m_method = Method::GET;
    }
    else
    {
        m_method = Method::UNKNOWN;
        return false; // Поддерживаем только GET
    }

    if (m_path.empty()) return false;

    // TODO: Parse headers if needed

    return true;
}

HttpRequest::Method HttpRequest::GetMethod() const { return m_method; }
const std::string& HttpRequest::GetPath() const { return m_path; }
