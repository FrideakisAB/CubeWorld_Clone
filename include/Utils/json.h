#ifndef JSON_H
#define JSON_H

#include "Log.h"
#include <string>
#include <variant>
#include "Utils/glm.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Utils {
    [[nodiscard]] std::string FileToString(std::ifstream&& file);
    [[nodiscard]] std::string FileToString(std::ifstream &file);
}

namespace json_utils {
    json TryParse(const std::string &str) noexcept;
}

namespace ns {
    void to_json(json& j, const glm::vec1& vector);
    void from_json(const json& j, glm::vec1& vector);

    void to_json(json& j, const glm::vec2& vector);
    void from_json(const json& j, glm::vec2& vector);

    void to_json(json& j, const glm::vec3& vector);
    void from_json(const json& j, glm::vec3& vector);

    void to_json(json& j, const glm::vec4& vector);
    void from_json(const json& j, glm::vec4& vector);

    void to_json(json& j, const glm::mat2& mat);
    void from_json(const json& j, glm::mat2& mat);

    void to_json(json& j, const glm::mat3& mat);
    void from_json(const json& j, glm::mat3& mat);

    void to_json(json& j, const glm::mat4& mat);
    void from_json(const json& j, glm::mat4& mat);
}

#endif
