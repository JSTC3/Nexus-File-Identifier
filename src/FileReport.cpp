#include "FileReport.hpp"

#include <ostream>

void printFileReport(std::ostream& output, const FileReport& report)
{
    if (!report.knownType)
    {
        output << "[?] " << report.filename << '\n'
               << "    Detected: " << report.detectedType << '\n';
        return;
    }

    if (report.extensionMatches)
    {
        output << "[OK] " << report.filename << '\n';
        return;
    }

    output << "[!] " << report.filename << '\n'
           << "    Detected: " << report.detectedType << '\n'
           << "    Extension: " << (report.extension.empty()
                ? "(none)" : report.extension) << '\n';
}