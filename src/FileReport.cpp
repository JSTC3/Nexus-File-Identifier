#include "FileReport.hpp"

#include <filesystem>
#include <ostream>

// ---------------------------------------------------------------------------
// printFileReport
//
// Three possible outcomes:
//
//   [OK]  filename                      — known type, extension matches
//   [?]   filename                      — unknown file type
//   [!]   filename  + full warning box  — known type, extension MISMATCH
// ---------------------------------------------------------------------------
void printFileReport(std::ostream& out, const FileReport& report)
{
    // Use only the filename portion for display, not the full path
    const std::string display =
        std::filesystem::path(report.filename).filename().string();

    // ── Unknown type ────────────────────────────────────────────────────────
    if (!report.knownType)
    {
        out << "[?] " << display << '\n'
            << "    Type    : " << report.detectedType << '\n'
            << '\n';
        return;
    }

    // ── Known type, extension OK ────────────────────────────────────────────
    if (report.extensionMatches)
    {
        out << "[OK] " << display
            << "  (" << report.detectedType << ")\n"
            << '\n';
        return;
    }

    // ── Known type, extension MISMATCH ─────────────────────────────────────
    const std::string ext =
        report.extension.empty() ? "(none)" : report.extension;

    const std::string expected =
        report.expectedExt.empty() ? "(no extension)" : report.expectedExt;

    out << "========================================\n"
        << "       NEXUS FILE IDENTIFIER\n"
        << "========================================\n"
        << '\n'
        << "File          : " << display << '\n'
        << "Extension     : " << ext << '\n'
        << "Detected Type : " << report.detectedType << '\n'
        << "Expected Ext  : " << expected << '\n'
        << '\n'
        << "[!] WARNING: Extension mismatch\n"
        << "[!] File content does not match extension\n"
        << '\n';
}
