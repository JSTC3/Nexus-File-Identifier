# NEXUS File Identifier

A small C++17 command-line tool that identifies files by their leading magic bytes and reports when the detected type disagrees with the filename extension.

The design uses `FileIdentifier` as the public analysis abstraction. `SignatureDatabase` owns signature rules through the abstract `FileSignature` interface, allowing matching behavior to be extended with new implementations. `MagicByteSignature` is the current implementation and overrides the matching operation.

## Build

Requirements: a C++17 compiler and GNU Make.

```sh
make
```

The executable is written to `build/nexus-fileid`.

## Usage

Run from the repository root to use the included signature database:

```sh
./build/nexus-fileid suspicious.jpeg
```

You can also provide a database path explicitly:

```sh
./build/nexus-fileid suspicious.jpeg signatures/signatures.txt
```

Example output:

```text
File: suspicious.jpeg
Detected type: JPEG
Expected extension: .jpg
Extension mismatch: yes
Actual extension: .jpeg
```

The database uses one rule per line in `TYPE|EXTENSION|HEX_BYTES` format. Blank lines and lines beginning with `#` are ignored. An empty extension means no extension is prescribed. For example:

```text
JPEG|.jpg|FF D8 FF
PNG|.png|89 50 4E 47 0D 0A 1A 0A
ELF executable||7F 45 4C 46
```

## Tests

```sh
make test
```

The tests create small temporary byte fixtures; the repository does not contain executable samples or malware.

## Scope and limitations

NEXUS compares signatures against the start of each file. It does not prove that a file is safe, parse or validate the full file format, inspect signatures at nonzero offsets, or detect every format. A matching magic number is an identification hint, not a security verdict. The current database is intentionally small and can be extended with additional rules.

## Why it matters

File extensions are user-controlled labels. Comparing them with a file's binary signature can help surface misleading names during triage, incident response, and basic security education. This utility is a lightweight inspection aid, not a malware scanner.