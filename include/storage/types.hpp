#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace storage {

using Integer = std::int64_t;
using Score = double;
using Members = std::vector<std::string>;

struct Value {
    std::variant<std::string, Integer, Members> data;
};

struct Record {
    Value value;
    std::optional<std::uint64_t> expires_at_ms;
};

} // namespace storage
