#include "FileIdentifier.hpp"

#include "Utils.hpp"

#include <cstddef>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace
{
bool isLikelyTextFile(const std::vector<unsigned char>& bytes)
{
    if (bytes.empty())
    {
        return false;
    }

    std::size_t printable = 0;
    std::size_t suspicious = 0;

    for (const unsigned char byte : bytes)
    {
        if (byte == '\0')
        {
            ++suspicious;
            continue;
        }

        if (byte == '\n' || byte == '\r' || byte == '\t' ||
            (byte >= 0x20 && byte <= 0x7E) || byte >= 0x80)
        {
            ++printable;
            continue;
        }

        ++suspicious;
    }

    if (suspicious == 0)
    {
        return true;
    }

    return static_cast<double>(printable) / static_cast<double>(bytes.size()) >= 0.9;
}
}

FileIdentifier::FileIdentifier(const std::string& signatureFile)
    : signatureDatabase(signatureFile)
{
}

IdentificationResult FileIdentifier::identify(const std::string& filename) const
{
    std::ifstream input(filename, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("could not open file: " + filename);
    }

    const std::vector<unsigned char> fileBytes(
        (std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const std::size_t headerLength = std::min(signatureDatabase.longestSignatureLength(), fileBytes.size());
    std::vector<unsigned char> header(
        fileBytes.begin(), fileBytes.begin() + static_cast<std::ptrdiff_t>(headerLength));

    IdentificationResult result;
    result.actualExtension = Utils::getExtension(filename);
    const FileSignature* signature = signatureDatabase.findMatch(header);
    if (signature != nullptr)
    {
        result.identified = true;
        result.fileType = signature->fileType();
        result.expectedExtension = signature->extension();
        result.extensionMismatch = !signature->extension().empty() &&
                                   !Utils::equalsIgnoreCase(result.actualExtension, signature->extension());
    }
    else if (isLikelyTextFile(fileBytes))
    {
        result.identified = true;
        result.fileType = "Text file";
        result.expectedExtension = ".txt";
        result.extensionMismatch = !Utils::equalsIgnoreCase(result.actualExtension, ".txt");
    }

    return result;
}