#include "MimeBuilder.h"
#include "../Common/Base64.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <random>

namespace fs = std::filesystem;

MimeBuilder::MimeBuilder(const Email& email) : m_email(email), m_boundary(GenerateBoundary())
{
}

std::string MimeBuilder::GenerateBoundary()
{
    std::string boundary = "----=_NextPart_";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);
    for (int i = 0; i < 16; ++i)
    {
        boundary += "0123456789ABCDEF"[distrib(gen)];
    }
    return boundary;
}

std::string MimeBuilder::GetMimeType(const std::string& filepath)
{
    fs::path path(filepath);
    std::string extension = path.extension().string();
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".png") return "image/png";
    if (extension == ".pdf") return "application/pdf";
    if (extension == ".txt") return "text/plain";
    return "application/octet-stream";
}

std::string MimeBuilder::Build()
{
    std::ostringstream oss;
    oss << "From: " << m_email.from << "\r\n";
    oss << "To: " << m_email.to << "\r\n";
    oss << "Subject: " << m_email.subject << "\r\n";
    oss << "MIME-Version: 1.0\r\n";

    if (m_email.attachments.empty())
    {
        oss << "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n";
        oss << m_email.body;
    }
    else
    {
        oss << "Content-Type: multipart/mixed; boundary=\"" << m_boundary << "\"\r\n\r\n";

        oss << "--" << m_boundary << "\r\n";
        oss << "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n";
        oss << m_email.body << "\r\n\r\n";

        for (const auto& attachment : m_email.attachments)
        {
            fs::path filePath(attachment.path);
            if (!fs::exists(filePath))
            {
                throw std::runtime_error("Attachment file not found: " + attachment.path);
            }
            std::ifstream file(filePath, std::ios::binary);
            std::vector<char> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            oss << "--" << m_boundary << "\r\n";
            oss << "Content-Type: " << GetMimeType(attachment.path) << "\r\n";
            oss << "Content-Transfer-Encoding: base64\r\n";
            oss << "Content-Disposition: attachment; filename=\"" << filePath.filename().string() << "\"\r\n\r\n";

            std::string encodedData = Base64::Encode(fileData);
            oss << encodedData << "\r\n";
        }

        oss << "--" << m_boundary << "--\r\n";
    }

    return oss.str();
}
