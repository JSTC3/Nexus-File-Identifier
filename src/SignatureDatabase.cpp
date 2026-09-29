#include "SignatureDatabase.hpp"

#include "Utils.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <utility>

namespace
{
std::string trim(const std::string& value)
{
    const std::string::size_type first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    const std::string::size_type last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> splitFields(const std::string& line)
{
    std::vector<std::string> fields;
    std::size_t start = 0;
    while (true)
    {
        const std::size_t delimiter = line.find('|', start);
        fields.push_back(trim(line.substr(start,
            delimiter == std::string::npos ? delimiter : delimiter - start)));
        if (delimiter == std::string::npos)
        {
            return fields;
        }
        start = delimiter + 1;
    }
}
}

SignatureDatabase::SignatureDatabase(const std::string& signatureFile)
{
    std::ifstream input(signatureFile);
    if (!input)
    {
        throw std::runtime_error("could not open signature database: " + signatureFile);
    }

    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line))
    {
        ++lineNumber;
        const std::string content = trim(line);
        if (content.empty() || content[0] == '#')
        {
            continue;
        }

        try
        {
            const std::vector<std::string> fields = splitFields(content);
            if (fields.size() != 3 || fields[0].empty() || fields[2].empty())
            {
                throw std::invalid_argument("expected TYPE|EXTENSION|HEX_BYTES");
            }
            if (!fields[1].empty() && fields[1][0] != '.')
            {
                throw std::invalid_argument("extension must be empty or begin with '.'");
            }

            std::unique_ptr<FileSignature> signature = std::make_unique<MagicByteSignature>(
                Utils::parseHexBytes(fields[2]), fields[0], fields[1]);
            maximumSignatureLength = std::max(maximumSignatureLength, signature->signatureLength());
            signatures.push_back(std::move(signature));
        }
        catch (const std::exception& error)
        {
            throw std::runtime_error("invalid signature database at line " + std::to_string(lineNumber) +
                                     ": " + error.what());
        }
    }

    if (signatures.empty())
    {
        throw std::runtime_error("signature database contains no signatures: " + signatureFile);
    }

    std::stable_sort(signatures.begin(), signatures.end(), [](const auto& left, const auto& right) {
        return left->signatureLength() > right->signatureLength();
    });
}

const FileSignature* SignatureDatabase::findMatch(const std::vector<unsigned char>& fileHeader) const noexcept
{
    const auto match = std::find_if(signatures.begin(), signatures.end(), [&fileHeader](const auto& signature) {
        return signature->matches(fileHeader);
    });
    return match == signatures.end() ? nullptr : match->get();
}

std::size_t SignatureDatabase::longestSignatureLength() const noexcept
{
    return maximumSignatureLength;
}