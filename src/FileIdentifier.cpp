#include "FileIdentifier.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>

// ---------------------------------------------------------------------------
// Constructor — signature database
//
// Each entry holds:
//   magicBytes   — the leading bytes that identify the format
//   fileType     — human-readable label shown in reports
//   extensions   — valid lowercase extensions for this format
//
// Empty extensions list means the format is expected to have NO extension
// (e.g. ELF executables on Linux).  Any file with an extension will be
// flagged as a mismatch.
// ---------------------------------------------------------------------------
FileIdentifier::FileIdentifier()
{
    signatures = {
        // ── Images ──────────────────────────────────────────────────────────
        {
            {0xFF, 0xD8, 0xFF},
            "JPEG image",
            {".jpg", ".jpeg"}
        },
        {
            {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
            "PNG image",
            {".png"}
        },
        {
            {0x47, 0x49, 0x46, 0x38},        // GIF87a / GIF89a
            "GIF image",
            {".gif"}
        },
        {
            {0x42, 0x4D},                    // BM
            "BMP image",
            {".bmp"}
        },
        {
            {0x49, 0x49, 0x2A, 0x00},        // TIFF little-endian
            "TIFF image",
            {".tif", ".tiff"}
        },
        {
            {0x4D, 0x4D, 0x00, 0x2A},        // TIFF big-endian
            "TIFF image",
            {".tif", ".tiff"}
        },
        {
            {0x52, 0x49, 0x46, 0x46},        // RIFF — disambiguate below by
            "RIFF container",                //  checking offset 8; kept simple
            {".wav", ".avi", ".webp"}        //  for now
        },
        // ── Documents ───────────────────────────────────────────────────────
        {
            {0x25, 0x50, 0x44, 0x46},        // %PDF
            "PDF document",
            {".pdf"}
        },
        // ── Archives ────────────────────────────────────────────────────────
        {
            {0x50, 0x4B, 0x03, 0x04},        // PK\x03\x04
            "ZIP archive",
            // DOCX, XLSX, PPTX, JAR, ODP … are all ZIP-based
            {".zip", ".docx", ".xlsx", ".pptx", ".jar", ".odt", ".ods",
             ".odp", ".apk", ".xpi"}
        },
        {
            {0x1F, 0x8B},                    // gzip magic
            "GZIP archive",
            {".gz", ".tgz"}
        },
        {
            {0x42, 0x5A, 0x68},              // BZ2
            "BZIP2 archive",
            {".bz2", ".tbz2"}
        },
        {
            {0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00},  // XZ
            "XZ archive",
            {".xz", ".txz"}
        },
        {
            {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07},  // Rar!
            "RAR archive",
            {".rar"}
        },
        {
            {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C},  // 7z
            "7-Zip archive",
            {".7z"}
        },
        // ── Audio / Video ────────────────────────────────────────────────────
        {
            {0x49, 0x44, 0x33},              // ID3 tag (most MP3 files)
            "MP3 audio",
            {".mp3"}
        },
        {
            {0xFF, 0xFB},                    // MPEG audio frame sync (no ID3)
            "MP3 audio",
            {".mp3"}
        },
        {
            {0x66, 0x74, 0x79, 0x70},        // ftyp box (MP4 / M4A / MOV …)
            "MP4/MOV video",
            {".mp4", ".m4v", ".m4a", ".mov", ".3gp"}
        },
        // ── Executables ─────────────────────────────────────────────────────
        {
            {0x4D, 0x5A},                    // MZ
            "Windows PE executable",
            {".exe", ".dll", ".sys", ".scr", ".ocx", ".cpl"}
        },
        {
            {0x7F, 0x45, 0x4C, 0x46},        // \x7fELF
            "ELF executable",
            // ELF binaries on Linux typically have NO extension.
            // An empty list means "no extension expected"; any extension is
            // treated as a mismatch.
            {}
        },
        {
            {0xCE, 0xFA, 0xED, 0xFE},        // Mach-O 32-bit little-endian
            "Mach-O executable",
            {}
        },
        {
            {0xCF, 0xFA, 0xED, 0xFE},        // Mach-O 64-bit little-endian
            "Mach-O executable",
            {}
        },
        {
            {0xCA, 0xFE, 0xBA, 0xBE},        // Mach-O fat binary / Java class
            "Mach-O fat binary / Java class",
            {".class"}
        },
        // ── Scripts / text ───────────────────────────────────────────────────
        {
            {0x23, 0x21},                    // #!  shebang
            "Script (shebang)",
            {".sh", ".py", ".rb", ".pl", ".bash", ".zsh"}
        },
    };
}

// ---------------------------------------------------------------------------
// readHeader — opens the file in binary mode and returns up to 16 bytes
// ---------------------------------------------------------------------------
std::vector<unsigned char> FileIdentifier::readHeader(
    const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        return {};

    std::vector<unsigned char> header(16);

    file.read(
        reinterpret_cast<char*>(header.data()),
        static_cast<std::streamsize>(header.size())
    );

    header.resize(static_cast<std::size_t>(file.gcount()));

    return header;
}

// ---------------------------------------------------------------------------
// analyze — core detection logic
//
// Returns a FileReport describing:
//   - the detected type
//   - the expected extension (first in the signature's list, or "" for ELF)
//   - whether the file's actual extension matches
// ---------------------------------------------------------------------------
FileReport FileIdentifier::analyze(const std::string& filename)
{
    // ── 1. Extract and normalise the file extension ──────────────────────
    std::string extension =
        std::filesystem::path(filename).extension().string();

    std::transform(
        extension.begin(), extension.end(), extension.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );

    // ── 2. Read the magic bytes ──────────────────────────────────────────
    std::vector<unsigned char> header = readHeader(filename);

    if (header.empty())
        return {filename, extension, "Unable to read file", "", false, false};

    // ── 3. Walk the signature database ──────────────────────────────────
    for (const FileSignature& sig : signatures)
    {
        if (header.size() < sig.magicBytes.size())
            continue;

        bool match = std::equal(
            sig.magicBytes.begin(), sig.magicBytes.end(),
            header.begin()
        );

        if (!match)
            continue;

        // The canonical "expected" extension is the first in the list,
        // or empty when none is defined (e.g. ELF).
        const std::string expectedExt =
            sig.extensions.empty() ? "" : sig.extensions.front();

        // Extension semantics:
        //   • sig.extensions is empty  → format expects NO extension;
        //     a file that has any extension is a mismatch.
        //   • sig.extensions is non-empty → the actual extension must be
        //     one of the listed values.
        bool extensionMatches;

        if (sig.extensions.empty())
        {
            // Matches only when the file has no extension at all
            extensionMatches = extension.empty();
        }
        else
        {
            extensionMatches =
                std::find(sig.extensions.begin(), sig.extensions.end(),
                          extension) != sig.extensions.end();
        }

        return {filename, extension, sig.fileType,
                expectedExt, extensionMatches, true};
    }

    // ── 4. No signature matched ──────────────────────────────────────────
    return {filename, extension, "Unknown file type", "", false, false};
}
