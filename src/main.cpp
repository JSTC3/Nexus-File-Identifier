#include "FileIdentifier.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./nexus-fileid <file>\n";
        return 1;
    }

    std::string filename = argv[1];

    FileIdentifier identifier;

    std::string result = identifier.identify(filename);

    std::cout << "File: " << filename << "\n";
    std::cout << "Detected type: " << result << "\n";

    return 0;
}