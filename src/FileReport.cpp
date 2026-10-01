#include "FileReport.hpp"

#include <filesystem>
#include <ostream>

// ---------------------------------------------------------------------------
// printFileReport
//
// Always prints the banner header and the file information block, then
// branches on the result:
//
//   [!] WARNING  — known type, extension MISMATCH
//   [OK]         — known type, extension matches
//   [?]          — type could not be identified
// ---------------------------------------------------------------------------
void printFileReport(std::ostream& out, const FileReport& report)
{
    // Use only the filename portion for display, not the full path
    const std::string display =
        std::filesystem::path(report.filename).filename().string();

    const std::string ext =
        report.extension.empty() ? "(none)" : report.extension;

    // For the expected-extension field:
    //   known type  → first valid extension, or "(no extension)" for ELF/Mach-O
    //   unknown     → "-" (no expectation can be stated)
    const std::string expected = !report.knownType
        ? "-"
        : report.expectedExt.empty()
            ? "(no extension)"
            : report.expectedExt;

    // ── Header ───────────────────────────────────────────────────────────────
    out << "========================================\n"
        << "        NEXUS FILE IDENTIFIER\n"
        << "========================================\n\n";

    // ── File information ─────────────────────────────────────────────────────
    out << "File          : " << display << '\n'
        << "Extension     : " << ext << '\n'
        << "Detected Type : " << report.detectedType << '\n'
        << "Expected Ext  : " << expected << "\n\n";

    // ── Extension mismatch ───────────────────────────────────────────────────
    if (report.knownType && !report.extensionMatches)
    {
        out << "[!] WARNING: Extension mismatch\n"
            << "[!] File content does not match extension\n\n";
        return;
    }

    // ── Known type, extension OK  /  Unknown type ────────────────────────────
    if (report.knownType)
        out << "[OK] " << display << " (" << report.detectedType << ")\n\n";
    else
        out << "[?] " << display << " — type not identified\n\n";
}
