#ifndef FILE_REPORT_HPP
#define FILE_REPORT_HPP

#include <iosfwd>
#include <string>

struct FileReport
{
    std::string filename;       // full path as given
    std::string extension;      // lowercase extension, e.g. ".jpg"
    std::string detectedType;   // human-readable type name
    std::string expectedExt;    // first valid extension for the detected type, or "" if any/none
    bool        extensionMatches;
    bool        knownType;
};

void printFileReport(std::ostream& output, const FileReport& report);

#endif
