#pragma once
#include "../Model/Email.h"
#include <string>

class MimeBuilder
{
public:
    explicit MimeBuilder(const Email& email);
    std::string Build();

private:
    std::string GetMimeType(const std::string& filepath);
    std::string GenerateBoundary();

    const Email& m_email;
    std::string m_boundary;
};
