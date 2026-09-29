#ifndef FILE_IDENTIFIER_HPP
#define FILE_IDENTIFIER_HPP

#include "FileSignature.hpp"

#include <string>
#include <vector>

class FileIdentifier
{
public:
    FileIdentifier();

    std::string identify(const std::string& filename);

private:
    std::vector<FileSignature> signatures;

    std::vector<unsigned char> readHeader(const std::string& filename);
};

#endif