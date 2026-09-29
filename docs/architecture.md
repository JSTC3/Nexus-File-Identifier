# Architecture

```text
main.cpp
  | creates FileIdentifier, parses arguments, prints result
  v
FileIdentifier
  | delegates byte matching to SignatureDatabase
  | reads only the number of leading bytes needed
  v
SignatureDatabase owns FileSignature objects
  | abstract matches() contract
  +--> MagicByteSignature implements prefix comparison
                  |
                  v
          IdentificationResult
          detected type and extension comparison
```

`FileSignature` is the polymorphic contract for one matching rule. `MagicByteSignature` encapsulates a type name, expected extension, and byte pattern, and implements the matching operation. `SignatureDatabase` owns these implementations with `std::unique_ptr`, validates the text database, and selects the longest matching signature when rules overlap. `FileIdentifier` composes the database and coordinates analysis. `Utils` owns small parsing and extension helpers.

The command-line entry point owns no detection logic. It accepts a file path and an optional signature database path, delegates analysis, and renders the result. File and database errors are reported to standard error with a nonzero exit status.

Detection is intentionally limited to signatures at offset zero. It does not validate the complete format or establish that a file is benign.