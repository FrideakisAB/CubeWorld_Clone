#include "Utils/json.h"

#include <fstream>

std::string Utils::FileToString(std::ifstream&& file)
{
    if (!file.is_open())
        return {};

    std::string src;
    getline(file, src, '\0' );
    file.close();

    return src;
}

std::string Utils::FileToString(std::ifstream &file)
{
    if (!file.is_open())
        return {};

    std::string src;
    getline(file, src, '\0' );

    return src;
}

json json_utils::TryParse(const std::string &str) noexcept
{
    json j;

    try
    {
        j = json::parse(str);
    }
    catch (const json::parse_error &ex)
    {
        logger->Error("JSON parse error! %s, with id: %i, in byte: %llu", ex.what(), ex.id, ex.byte);
    }

    return j;
}

namespace ns {
    void to_json(json& j, const glm::vec1& vector)
    {
        j = json{{"value", vector.x}};
    }

    void from_json(const json& j, glm::vec1& vector)
    {
        j.at("value").get_to(vector.x);
    }

    void to_json(json& j, const glm::vec2& vector)
    {
        j = json{{"value1", vector.x}, {"value2", vector.y}};
    }

    void from_json(const json& j, glm::vec2& vector)
    {
        j.at("value1").get_to(vector.x);
        j.at("value2").get_to(vector.y);
    }

    void to_json(json& j, const glm::vec3& vector)
    {
        j = json{{"value1", vector.x}, {"value2", vector.y}, {"value3", vector.z}};
    }

    void from_json(const json& j, glm::vec3& vector)
    {
        j.at("value1").get_to(vector.x);
        j.at("value2").get_to(vector.y);
        j.at("value3").get_to(vector.z);
    }

    void to_json(json& j, const glm::vec4& vector)
    {
        j = json{{"value1", vector.x}, {"value2", vector.y}, {"value3", vector.z}, {"value4", vector.w}};
    }

    void from_json(const json& j, glm::vec4& vector)
    {
        j.at("value1").get_to(vector.x);
        j.at("value2").get_to(vector.y);
        j.at("value3").get_to(vector.z);
        j.at("value4").get_to(vector.w);
    }

    void to_json(json& j, const glm::mat2& mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[1][0], mat[1][1]}}};
    }

    void from_json(const json& j, glm::mat2& mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[1][0] = j["value"][2];
        mat[1][1] = j["value"][3];
    }

    void to_json(json& j, const glm::mat3& mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[0][2],
                                   mat[1][0], mat[1][1], mat[1][2],
                                   mat[2][0], mat[2][1], mat[2][2]}}};
    }

    void from_json(const json& j, glm::mat3& mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[0][2] = j["value"][2];

        mat[1][0] = j["value"][3];
        mat[1][1] = j["value"][4];
        mat[1][2] = j["value"][5];

        mat[2][0] = j["value"][6];
        mat[2][1] = j["value"][7];
        mat[2][2] = j["value"][8];
    }

    void to_json(json& j, const glm::mat4& mat)
    {
        j = json{{"value", {mat[0][0], mat[0][1], mat[0][2], mat[0][3],
                                   mat[1][0], mat[1][1], mat[1][2], mat[1][3],
                                   mat[2][0], mat[2][1], mat[2][2], mat[2][3],
                                   mat[3][0], mat[3][1], mat[3][2], mat[3][3]}}};
    }

    void from_json(const json& j, glm::mat4& mat)
    {
        mat[0][0] = j["value"][0];
        mat[0][1] = j["value"][1];
        mat[0][2] = j["value"][2];
        mat[0][3] = j["value"][3];

        mat[1][0] = j["value"][4];
        mat[1][1] = j["value"][5];
        mat[1][2] = j["value"][6];
        mat[1][3] = j["value"][7];

        mat[2][0] = j["value"][8];
        mat[2][1] = j["value"][9];
        mat[2][2] = j["value"][10];
        mat[2][3] = j["value"][11];

        mat[3][0] = j["value"][12];
        mat[3][1] = j["value"][13];
        mat[3][2] = j["value"][14];
        mat[3][3] = j["value"][15];
    }
}
