#include "Utils.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace Utils
{
std::string getExtension(const std::string& filename)
{
    const std::string::size_type separator = filename.find_last_of("/\\");
    const std::string::size_type basenameStart = separator == std::string::npos ? 0 : separator + 1;
    const std::string::size_type dot = filename.find_last_of('.');

    if (dot == std::string::npos || dot < basenameStart || dot == basenameStart)
    {
        return "";
    }

    std::string extension = filename.substr(dot);
    for (char& character : extension)
    {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return extension;
}

std::vector<unsigned char> parseHexBytes(const std::string& value)
{
    std::istringstream input(value);
    std::vector<unsigned char> bytes;
    std::string token;

    while (input >> token)
    {
        if (token.size() != 2)
        {
            throw std::invalid_argument("each signature byte must contain exactly two hexadecimal digits");
        }

        std::size_t parsedCharacters = 0;
        const unsigned long byte = std::stoul(token, &parsedCharacters, 16);
        if (parsedCharacters != token.size() || byte > 0xFF)
        {
            throw std::invalid_argument("signature contains an invalid hexadecimal byte: " + token);
        }
        bytes.push_back(static_cast<unsigned char>(byte));
    }

    if (bytes.empty())
    {
        throw std::invalid_argument("signature must contain at least one byte");
    }
    return bytes;
}

bool equalsIgnoreCase(const std::string& left, const std::string& right)
{
    if (left.size() != right.size())
    {
        return false;
    }

    for (std::size_t index = 0; index < left.size(); ++index)
    {
        if (std::tolower(static_cast<unsigned char>(left[index])) !=
            std::tolower(static_cast<unsigned char>(right[index])))
        {
            return false;
        }
    }
    return true;
}
}