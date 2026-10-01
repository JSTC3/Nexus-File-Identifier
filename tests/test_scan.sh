#!/bin/sh
# test_scan.sh — integration tests for nexus-fileid
# Covers V1 (magic-byte detection), V2 (extension mismatch), V3 (dir scan).
set -eu

PASS=0
FAIL=0

check()
{
    label="$1"
    result="$2"
    if [ "$result" = "0" ]; then
        printf '[PASS] %s\n' "$label"
        PASS=$((PASS + 1))
    else
        printf '[FAIL] %s\n' "$label"
        FAIL=$((FAIL + 1))
    fi
}

temporary_directory=$(mktemp -d)
trap 'rm -rf "$temporary_directory"' EXIT HUP INT TERM

# ── Fixture files ─────────────────────────────────────────────────────────
mkdir -p "$temporary_directory/nested"

printf '\377\330\377' > "$temporary_directory/photo.jpg"    # real JPEG
printf 'MZ'           > "$temporary_directory/evil.jpeg"    # PE disguised as JPEG
printf 'unknown'      > "$temporary_directory/nested/notes.bin"  # unknown

# ── V1: single-file — always shows the banner header ─────────────────────
single=$(./nexus-fileid "$temporary_directory/photo.jpg")

printf '%s\n' "$single" | grep -qF 'NEXUS FILE IDENTIFIER'
check "V1: banner header present"             "$?"

printf '%s\n' "$single" | grep -qF 'photo.jpg'
check "V1: filename in report"                "$?"

printf '%s\n' "$single" | grep -qF 'JPEG image'
check "V1: detected type correct"             "$?"

printf '%s\n' "$single" | grep -qF '[OK]'
check "V1: OK result line present"            "$?"

# ── V2: extension mismatch banner ────────────────────────────────────────
mismatch=$(./nexus-fileid "$temporary_directory/evil.jpeg")

printf '%s\n' "$mismatch" | grep -qF 'NEXUS FILE IDENTIFIER'
check "V2: banner header present"             "$?"

printf '%s\n' "$mismatch" | grep -qF 'evil.jpeg'
check "V2: filename in report"                "$?"

printf '%s\n' "$mismatch" | grep -qF 'Windows PE executable'
check "V2: detected type correct"             "$?"

printf '%s\n' "$mismatch" | grep -qF 'Expected Ext  : .exe'
check "V2: expected extension shown"          "$?"

printf '%s\n' "$mismatch" | grep -qF '[!] WARNING: Extension mismatch'
check "V2: WARNING line present"              "$?"

# ── V2: unknown file ─────────────────────────────────────────────────────
unknown=$(./nexus-fileid "$temporary_directory/nested/notes.bin")

printf '%s\n' "$unknown" | grep -qF 'NEXUS FILE IDENTIFIER'
check "V2 unknown: banner header present"     "$?"

printf '%s\n' "$unknown" | grep -qF 'Expected Ext  : -'
check "V2 unknown: expected ext shows dash"   "$?"

printf '%s\n' "$unknown" | grep -qF '[?]'
check "V2 unknown: [?] result line present"   "$?"

# ── V3: directory scan ────────────────────────────────────────────────────
scan=$(./nexus-fileid "$temporary_directory")

printf '%s\n' "$scan" | grep -qF 'photo.jpg'
check "V3 dir scan: OK file appears"          "$?"

printf '%s\n' "$scan" | grep -qF '[OK]'
check "V3 dir scan: OK result line present"   "$?"

printf '%s\n' "$scan" | grep -qF 'evil.jpeg'
check "V3 dir scan: mismatch file appears"    "$?"

printf '%s\n' "$scan" | grep -qF '[!] WARNING: Extension mismatch'
check "V3 dir scan: mismatch WARNING present" "$?"

printf '%s\n' "$scan" | grep -qF 'notes.bin'
check "V3 dir scan: unknown file appears"     "$?"

printf '%s\n' "$scan" | grep -qF '[?]'
check "V3 dir scan: [?] result present"       "$?"

printf '%s\n' "$scan" | grep -qF 'Files scanned: 3'
check "V3 summary: files scanned = 3"         "$?"

printf '%s\n' "$scan" | grep -qF 'Mismatches:    1'
check "V3 summary: mismatches = 1"            "$?"

printf '%s\n' "$scan" | grep -qF 'Unknown:       1'
check "V3 summary: unknown = 1"               "$?"

# ── Results ───────────────────────────────────────────────────────────────
printf '\n%d passed, %d failed\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
