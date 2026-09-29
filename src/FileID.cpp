#include "FileIdentifier.hpp"

#include <fstream>

FileIdentifier::FileIdentifier()
{
    signatures = {
        {
            {0xFF, 0xD8, 0xFF},
            "JPEG image",
            ".jpg/.jpeg"
        },
        {
            {0x89, 0x50, 0x4E, 0x47},
            "PNG image",
            ".png"
        },
        {
            {0x25, 0x50, 0x44, 0x46},
            "PDF document",
            ".pdf"
        },
        {
            {0x50, 0x4B, 0x03, 0x04},
            "ZIP archive",
            ".zip"
        },
        {
            {0x4D, 0x5A},
            "Windows PE executable",
            ".exe"
        },
        {
            {0x7F, 0x45, 0x4C, 0x46},
            "ELF executable",
            ""
        }
    };
}

std::vector<unsigned char> FileIdentifier::readHeader(
    const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        return {};

    std::vector<unsigned char> header(16);

    file.read(
        reinterpret_cast<char*>(header.data()),
        header.size()
    );

    header.resize(file.gcount());

    return header;
}

std::string FileIdentifier::identify(const std::string& filename)
{
    std::vector<unsigned char> header = readHeader(filename);

    if (header.empty())
        return "Unable to read file";

    for (const FileSignature& signature : signatures)
    {
        if (header.size() < signature.magicBytes.size())
            continue;

        bool match = true;

        for (size_t i = 0; i < signature.magicBytes.size(); i++)
        {
            if (header[i] != signature.magicBytes[i])
            {
                match = false;
                break;
            }
        }

        if (match)
            return signature.fileType;
    }

    return "Unknown file type";
}