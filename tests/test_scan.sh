#!/bin/sh
set -eu

temporary_directory=$(mktemp -d)
trap 'rm -rf "$temporary_directory"' EXIT HUP INT TERM

mkdir -p "$temporary_directory/nested"
printf '\377\330\377' > "$temporary_directory/photo.jpg"
printf 'MZ' > "$temporary_directory/suspicious.jpeg"
printf 'unknown' > "$temporary_directory/nested/notes.bin"

single_file_output=$(./nexus-fileid "$temporary_directory/photo.jpg")
printf '%s\n' "$single_file_output" | grep -F '[OK] '

scan_output=$(./nexus-fileid "$temporary_directory")
printf '%s\n' "$scan_output" | grep -F '[OK] ' | grep -F 'photo.jpg'
printf '%s\n' "$scan_output" | grep -F '[!] ' | grep -F 'suspicious.jpeg'
printf '%s\n' "$scan_output" | grep -F '[?] ' | grep -F 'notes.bin'
printf '%s\n' "$scan_output" | grep -F 'Files scanned: 3'
printf '%s\n' "$scan_output" | grep -F 'Mismatches:    1'
printf '%s\n' "$scan_output" | grep -F 'Unknown:       1'

printf 'Recursive scan tests passed.\n'