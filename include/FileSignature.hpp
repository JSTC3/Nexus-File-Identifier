#ifndef FILE_SIGNATURE_HPP
#define FILE_SIGNATURE_HPP

#include <string>
#include <vector>

struct FileSignature
{
    std::vector<unsigned char> magicBytes;
    std::string fileType;
    std::vector<std::string> extensions;
};

#endif