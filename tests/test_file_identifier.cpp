#include "FileIdentifier.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{
void writeText(const std::filesystem::path& path, const std::string& text)
{
    std::ofstream output(path);
    output << text;
}

void writeBytes(const std::filesystem::path& path, const std::vector<unsigned char>& bytes)
{
    std::ofstream output(path, std::ios::binary);
    if (!bytes.empty())
    {
        output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    }
}
}

int main()
{
    const std::filesystem::path temporaryDirectory = std::filesystem::temp_directory_path() /
        ("nexus-fileid-tests-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(temporaryDirectory);

    const std::filesystem::path database = temporaryDirectory / "signatures.txt";
    writeText(database,
        "# Test signatures\n"
        "JPEG|.jpg|FF D8 FF\n"
        "JPEG prefix|.prefix|FF D8\n"
        "PNG|.png|89 50 4E 47 0D 0A 1A 0A\n"
        "PDF|.pdf|25 50 44 46\n"
        "PE executable|.exe|4D 5A\n");

    const std::filesystem::path jpeg = temporaryDirectory / "photo.JPG";
    const std::filesystem::path png = temporaryDirectory / "image.png";
    const std::filesystem::path pdf = temporaryDirectory / "report.pdf";
    const std::filesystem::path renamed = temporaryDirectory / "invoice.txt";
    const std::filesystem::path unknown = temporaryDirectory / "unknown.bin";
    writeBytes(jpeg, {0xFF, 0xD8, 0xFF, 0x00});
    writeBytes(png, {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A});
    writeBytes(pdf, {0x25, 0x50, 0x44, 0x46, 0x2D});
    writeBytes(renamed, {0x4D, 0x5A, 0x90, 0x00});
    writeBytes(unknown, {0x00, 0x01, 0x02, 0x03});

    int passed = 0;
    int failed = 0;
    const auto check = [&passed, &failed](const std::string& name, bool condition) {
        if (condition)
        {
            ++passed;
            std::cout << "[PASS] " << name << '\n';
        }
        else
        {
            ++failed;
            std::cout << "[FAIL] " << name << '\n';
        }
    };

    try
    {
        const FileIdentifier identifier(database.string());
        const IdentificationResult jpegResult = identifier.identify(jpeg.string());
        const IdentificationResult pngResult = identifier.identify(png.string());
        const IdentificationResult pdfResult = identifier.identify(pdf.string());
        const IdentificationResult renamedResult = identifier.identify(renamed.string());
        const IdentificationResult unknownResult = identifier.identify(unknown.string());

        check("most-specific JPEG rule and case-insensitive extension", jpegResult.identified &&
            jpegResult.fileType == "JPEG" && !jpegResult.extensionMismatch);
        check("PNG detection", pngResult.identified && pngResult.fileType == "PNG");
        check("PDF detection", pdfResult.identified && pdfResult.fileType == "PDF");
        check("PE detection and extension mismatch", renamedResult.identified &&
            renamedResult.fileType == "PE executable" && renamedResult.extensionMismatch);
        check("unknown file", !unknownResult.identified && unknownResult.fileType.empty());

        const std::filesystem::path invalidDatabase = temporaryDirectory / "invalid.txt";
        writeText(invalidDatabase, "JPEG|.jpg|FF D8 GG\n");
        bool rejectedInvalidDatabase = false;
        try
        {
            const FileIdentifier invalidIdentifier(invalidDatabase.string());
            (void)invalidIdentifier;
        }
        catch (const std::exception&)
        {
            rejectedInvalidDatabase = true;
        }
        check("invalid signature database is rejected", rejectedInvalidDatabase);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Test setup or execution error: " << error.what() << '\n';
        std::filesystem::remove_all(temporaryDirectory);
        return 1;
    }

    std::filesystem::remove_all(temporaryDirectory);
    std::cout << passed << '/' << (passed + failed) << " tests passed\n";
    return failed == 0 ? 0 : 1;
}