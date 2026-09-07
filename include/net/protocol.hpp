#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace net {
std::vector<std::uint8_t> frame(const std::string& payload);
bool extract(std::vector<std::uint8_t>& buffer,std::string& payload);
std::vector<std::string> split(const std::string& s);
}