#pragma once

#include "SignatureDatabase.hpp"

#include <string>

struct IdentificationResult
{
    bool identified = false;
    std::string fileType;
    std::string expectedExtension;
    std::string actualExtension;
    bool extensionMismatch = false;
};

class FileIdentifier
{
public:
    explicit FileIdentifier(const std::string& signatureFile = "signatures/signatures.txt");

    IdentificationResult identify(const std::string& filename) const;

private:
    SignatureDatabase signatureDatabase;
};