#include "FileIdentifier.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./nexus-fileid <file-or-directory>\n";
        return 1;
    }

    const std::filesystem::path inputPath = argv[1];
    FileIdentifier identifier;

    std::error_code error;
    const bool isDirectory = std::filesystem::is_directory(inputPath, error);

    if (!isDirectory)
    {
        printFileReport(std::cout, identifier.analyze(inputPath.string()));
        return 0;
    }

    std::cout << "Scanning: " << inputPath.string() << "\n\n";

    std::size_t filesScanned = 0;
    std::size_t mismatches = 0;
    std::size_t unknown = 0;

    try
    {
        for (const auto& entry :
            std::filesystem::recursive_directory_iterator(inputPath))
        {
            if (!entry.is_regular_file())
                continue;

            FileReport report = identifier.analyze(entry.path().string());
            printFileReport(std::cout, report);
            ++filesScanned;

            if (!report.knownType)
                ++unknown;
            else if (!report.extensionMatches)
                ++mismatches;
        }
    }
    catch (const std::filesystem::filesystem_error& exception)
    {
        std::cerr << "Unable to scan directory: " << exception.what() << '\n';
        return 1;
    }

    std::cout << "\n--------------------------------\n"
              << "Files scanned: " << filesScanned << '\n'
              << "Mismatches:    " << mismatches << '\n'
              << "Unknown:       " << unknown << '\n'
              << "--------------------------------\n";

    return 0;
}