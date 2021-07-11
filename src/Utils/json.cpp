#include "Utils/json.h"

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