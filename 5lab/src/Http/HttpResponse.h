#pragma once
#include <string>
#include <vector>
#include <map>

class HttpResponse
{
public:
    void SetStatusCode(int code, const std::string& message);
    void SetHeader(const std::string& name, const std::string& value);
    void SetBody(const std::vector<char>& body);

    std::string BuildResponse() const;

    static HttpResponse StockResponse(int statusCode);

private:
    int m_statusCode = 200;
    std::string m_statusMessage = "OK";
    std::map<std::string, std::string> m_headers;
    std::vector<char> m_body;
};
