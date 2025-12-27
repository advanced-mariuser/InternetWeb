#include "CacheManager.h"
#include <fstream>
#include <filesystem>
#include <openssl/sha.h>
#include <iomanip>

namespace fs = std::filesystem;

CacheManager::CacheManager(std::string cacheDir) : m_cacheDir(std::move(cacheDir))
{
    if (!fs::exists(m_cacheDir))
    {
        fs::create_directories(m_cacheDir);
    }
}

std::string CacheManager::GenerateFilePath(const std::string& key)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, key.c_str(), key.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return (fs::path(m_cacheDir) / ss.str()).string();
}

std::optional<std::vector<char>> CacheManager::Get(const std::string& key)
{
    std::string filePath = GenerateFilePath(key);
    if (fs::exists(filePath))
    {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return std::nullopt;

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size))
        {
            return buffer;
        }
    }
    return std::nullopt;
}

void CacheManager::Put(const std::string& key, const std::vector<char>& data)
{
    std::string filePath = GenerateFilePath(key);
    std::ofstream file(filePath, std::ios::binary);
    file.write(data.data(), data.size());
}
