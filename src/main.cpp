#include "FileIdentifier.hpp"

#include <exception>
#include <iostream>
#include <string>

namespace
{
void printUsage(const char* program)
{
    std::cerr << "Usage: " << program << " <file> [signatures-file]\n";
}
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        printUsage(argv[0]);
        return 2;
    }

    try
    {
        const std::string signatureFile = argc == 3 ? argv[2] : "signatures/signatures.txt";
        const FileIdentifier identifier(signatureFile);
        const IdentificationResult result = identifier.identify(argv[1]);

        std::cout << "File: " << argv[1] << '\n';
        if (result.identified)
        {
            std::cout << "Detected type: " << result.fileType << '\n';
            std::cout << "Expected extension: "
                      << (result.expectedExtension.empty() ? "(any)" : result.expectedExtension) << '\n';
            std::cout << "Extension mismatch: " << (result.extensionMismatch ? "yes" : "no") << '\n';
        }
        else
        {
            std::cout << "Detected type: Unknown\n";
        }
        std::cout << "Actual extension: "
                  << (result.actualExtension.empty() ? "(none)" : result.actualExtension) << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}