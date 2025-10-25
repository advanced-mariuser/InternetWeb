#include "HttpResponse.h"
#include <sstream>

void HttpResponse::SetStatusCode(int code, const std::string& message)
{
    m_statusCode = code;
    m_statusMessage = message;
}

void HttpResponse::SetHeader(const std::string& name, const std::string& value)
{
    m_headers[name] = value;
}

void HttpResponse::SetBody(const std::vector<char>& body)
{
    m_body = body;
    SetHeader("Content-Length", std::to_string(m_body.size()));
}

std::string HttpResponse::BuildResponse() const
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << m_statusCode << " " << m_statusMessage << "\r\n";
    for (const auto& [name, value] : m_headers)
    {
        responseStream << name << ": " << value << "\r\n";
    }
    responseStream << "\r\n";

    std::string responseStr = responseStream.str();
    responseStr.append(m_body.begin(), m_body.end());

    return responseStr;
}

HttpResponse HttpResponse::StockResponse(int statusCode)
{
    HttpResponse response;
    if (statusCode == 404)
    {
        response.SetStatusCode(404, "Not Found");
        response.SetHeader("Content-Type", "text/plain");
        std::string body_str = "File Not Found";
        std::vector<char> body(body_str.begin(), body_str.end());
        response.SetBody(body);
    } else if (statusCode == 400)
    {
        response.SetStatusCode(400, "Bad Request");
        response.SetHeader("Content-Type", "text/plain");
        std::string body_str = "400 Bad Request";
        response.SetBody({body_str.begin(), body_str.end()});
    }
    // Можно добавить другие стандартные ответы, например, 400 Bad Request
    return response;
}
