CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS ?=

SRC := $(shell find src -name '*.cpp')
OBJ := $(SRC:src/%.cpp=build/%.o)

all: bin/storage_server bin/storage_client

bin/storage_server: $(OBJ) | bin
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)

bin/storage_client: client/client.cpp | bin
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -rf build bin

.PHONY: all clean
