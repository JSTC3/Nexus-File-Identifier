#include "FileSignature.hpp"

#include <algorithm>
#include <utility>

MagicByteSignature::MagicByteSignature(
    std::vector<unsigned char> magicBytes,
    std::string fileType,
    std::string extension)
    : magicBytes(std::move(magicBytes)),
      detectedType(std::move(fileType)),
      expectedExtension(std::move(extension))
{
}

bool MagicByteSignature::matches(const std::vector<unsigned char>& fileHeader) const noexcept
{
    return fileHeader.size() >= magicBytes.size() &&
           std::equal(magicBytes.begin(), magicBytes.end(), fileHeader.begin());
}

const std::string& MagicByteSignature::fileType() const noexcept
{
    return detectedType;
}

const std::string& MagicByteSignature::extension() const noexcept
{
    return expectedExtension;
}

std::size_t MagicByteSignature::signatureLength() const noexcept
{
    return magicBytes.size();
}