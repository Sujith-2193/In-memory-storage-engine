CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude -pthread
SRC := $(shell find src -name '*.cpp')
OBJ := $(SRC:src/%.cpp=build/%.o)
all: bin/storage_server bin/storage_client bin/storage_tests
bin/storage_server: $(OBJ) | bin
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@
bin/storage_client: client/client.cpp | bin
	$(CXX) $(CXXFLAGS) $< src/net/protocol.cpp -o $@
bin/storage_tests: tests/test_storage.cpp src/storage/engine.cpp | bin
	$(CXX) $(CXXFLAGS) $^ -o $@
build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
bin:
	mkdir -p bin
clean:
	rm -rf build bin
.PHONY: all clean
