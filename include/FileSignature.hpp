#pragma once

#include <cstddef>
#include <string>
#include <vector>

class FileSignature
{
public:
    virtual ~FileSignature() = default;

    virtual bool matches(const std::vector<unsigned char>& fileHeader) const noexcept = 0;
    virtual const std::string& fileType() const noexcept = 0;
    virtual const std::string& extension() const noexcept = 0;
    virtual std::size_t signatureLength() const noexcept = 0;
};

class MagicByteSignature final : public FileSignature
{
public:
    MagicByteSignature(std::vector<unsigned char> magicBytes, std::string fileType, std::string extension);

    bool matches(const std::vector<unsigned char>& fileHeader) const noexcept override;
    const std::string& fileType() const noexcept override;
    const std::string& extension() const noexcept override;
    std::size_t signatureLength() const noexcept override;

private:
    std::vector<unsigned char> magicBytes;
    std::string detectedType;
    std::string expectedExtension;
};