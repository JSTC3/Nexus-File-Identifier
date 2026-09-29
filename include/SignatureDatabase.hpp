#pragma once

#include "FileSignature.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class SignatureDatabase
{
public:
    explicit SignatureDatabase(const std::string& signatureFile);

    const FileSignature* findMatch(const std::vector<unsigned char>& fileHeader) const noexcept;
    std::size_t longestSignatureLength() const noexcept;

private:
    std::vector<std::unique_ptr<FileSignature>> signatures;
    std::size_t maximumSignatureLength = 0;
};