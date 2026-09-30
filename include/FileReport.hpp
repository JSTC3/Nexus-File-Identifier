#ifndef FILE_REPORT_HPP
#define FILE_REPORT_HPP

#include <iosfwd>
#include <string>

struct FileReport
{
    std::string filename;
    std::string extension;
    std::string detectedType;
    bool extensionMatches;
    bool knownType;
};

void printFileReport(std::ostream& output, const FileReport& report);

#endif