#pragma once

#include <string>
#include <vector>

namespace Utils
{
std::string getExtension(const std::string& filename);
std::vector<unsigned char> parseHexBytes(const std::string& value);
bool equalsIgnoreCase(const std::string& left, const std::string& right);
}