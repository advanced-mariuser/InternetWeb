#pragma once
#include <string>
#include <vector>
#include <optional>

class CacheManager
{
public:
    explicit CacheManager(std::string cacheDir);
    std::optional<std::vector<char>> Get(const std::string& key);
    void Put(const std::string& key, const std::vector<char>& data);

private:
    std::string GenerateFilePath(const std::string& key);
    std::string m_cacheDir;
};
