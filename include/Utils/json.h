#ifndef JSON_H
#define JSON_H

#include "Log.h"
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Utils {
    std::string FileToString(std::ifstream&& file);
}

namespace json_utils {
    json TryParse(const std::string &str) noexcept;
}

#endif
