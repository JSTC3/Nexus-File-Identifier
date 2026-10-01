#!/bin/sh
# test_scan.sh — integration tests for nexus-fileid
# Tests V1 (magic-byte detection), V2 (extension mismatch), V3 (dir scan).
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

# Real JPEG
printf '\377\330\377' > "$temporary_directory/photo.jpg"
# PE binary disguised as JPEG
printf 'MZ'           > "$temporary_directory/suspicious.jpeg"
# Completely unknown bytes
printf 'unknown'      > "$temporary_directory/nested/notes.bin"

# ── V1: single-file detection ─────────────────────────────────────────────
single=$(./nexus-fileid "$temporary_directory/photo.jpg")
printf '%s\n' "$single" | grep -qF '[OK]'
check "V1 single file: JPEG recognised as OK" "$?"

# ── V2: single-file mismatch banner ──────────────────────────────────────
mismatch=$(./nexus-fileid "$temporary_directory/suspicious.jpeg")

printf '%s\n' "$mismatch" | grep -qF 'NEXUS FILE IDENTIFIER'
check "V2 mismatch: banner header present"    "$?"

printf '%s\n' "$mismatch" | grep -qF 'suspicious.jpeg'
check "V2 mismatch: filename in report"       "$?"

printf '%s\n' "$mismatch" | grep -qF 'Windows PE executable'
check "V2 mismatch: detected type correct"    "$?"

printf '%s\n' "$mismatch" | grep -qF 'Extension mismatch'
check "V2 mismatch: warning line present"     "$?"

# ── V3: directory scan ────────────────────────────────────────────────────
scan=$(./nexus-fileid "$temporary_directory")

# photo.jpg — OK line
printf '%s\n' "$scan" | grep -qF '[OK]'
check "V3 dir scan: OK file reported"         "$?"

# suspicious.jpeg — mismatch detected somewhere in scan output
printf '%s\n' "$scan" | grep -qF 'suspicious.jpeg'
check "V3 dir scan: mismatch file in output"  "$?"

# notes.bin — unknown
printf '%s\n' "$scan" | grep -qF '[?]'
check "V3 dir scan: unknown file reported"    "$?"

# Summary counters
printf '%s\n' "$scan" | grep -qF 'Files scanned: 3'
check "V3 summary: files scanned = 3"         "$?"

printf '%s\n' "$scan" | grep -qF 'Mismatches:    1'
check "V3 summary: mismatches = 1"            "$?"

printf '%s\n' "$scan" | grep -qF 'Unknown:       1'
check "V3 summary: unknown = 1"               "$?"

# ── Results ───────────────────────────────────────────────────────────────
printf '\n%d passed, %d failed\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
