CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude

APP = build/nexus-fileid
TEST_APP = build/test_file_identifier
LIB_SOURCES = src/FileIdentifier.cpp src/FileSignature.cpp src/SignatureDatabase.cpp src/Utils.cpp

.PHONY: all test clean

all: $(APP)

$(APP): src/main.cpp $(LIB_SOURCES) include/FileIdentifier.hpp include/FileSignature.hpp include/SignatureDatabase.hpp include/Utils.hpp signatures/signatures.txt
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp $(LIB_SOURCES) -o $(APP)

$(TEST_APP): tests/test_file_identifier.cpp $(LIB_SOURCES) include/FileIdentifier.hpp include/FileSignature.hpp include/SignatureDatabase.hpp include/Utils.hpp
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_file_identifier.cpp $(LIB_SOURCES) -o $(TEST_APP)

test: $(TEST_APP)
	./$(TEST_APP)

clean:
	rm -rf build