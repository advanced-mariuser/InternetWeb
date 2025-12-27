#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace Base64
{
    inline std::string Encode(const std::vector<char>& data)
    {
        static const std::string base64Chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        std::string ret;
        int i = 0;
        unsigned char charArray3[3];
        unsigned char charArray4[4];

        auto bytesToEncode = reinterpret_cast<const unsigned char*>(data.data());
        size_t inputLength = data.size();

        while (inputLength--)
        {
            charArray3[i++] = *(bytesToEncode++);
            if (i == 3)
            {
                charArray4[0] = (charArray3[0] & 0xfc) >> 2;
                charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
                charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
                charArray4[3] = charArray3[2] & 0x3f;
                for (i = 0; (i < 4); i++) ret += base64Chars[charArray4[i]];
                i = 0;
            }
        }
        if (i)
        {
            for (int j = i; j < 3; j++)
            {
                charArray3[j] = '\0';
            }
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            for (int j = 0; (j < i + 1); j++)
            {
                ret += base64Chars[charArray4[j]];
            }
            while ((i++ < 3))
            {
                ret += '=';
            }
        }
        return ret;
    }

    inline std::string Encode(const std::string& data)
    {
        const std::vector<char> input(data.begin(), data.end());
        return Encode(input);
    }
}
