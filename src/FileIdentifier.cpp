#include "FileIdentifier.hpp"

#include "Utils.hpp"

#include <fstream>
#include <stdexcept>

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

    std::vector<unsigned char> header(signatureDatabase.longestSignatureLength());
    input.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(header.size()));
    header.resize(static_cast<std::size_t>(input.gcount()));

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

    return result;
}