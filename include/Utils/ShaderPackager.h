#ifndef SHADERPACKAGER_H
#define SHADERPACKAGER_H

#include <string>
#include <variant>
#include "Platform.h"
#include "Utils/glm.h"
#include "Utils/json.h"

namespace Utils {
    enum class ShaderValue : u8 {
        Int = 0,
        UnsignedInt,
        Float,
        Double,
        Vector2,
        Vector3,
        Vector4,
        Mat2,
        Mat3,
        Mat4,
        Sampler1D,
        Sampler2D,
        Sampler3D,
        SamplerCube
    };

    struct ShaderParam {
        std::string name;
        ShaderValue valueType;
    };

    struct ShaderParamValue {
        ShaderValue valueType;
        std::variant<i32, u32, f32, f64,
                     glm::vec2, glm::vec3, glm::vec4,
                     glm::mat2, glm::mat3, glm::mat4> value;
    };

    class ShaderPackager {

    };
}

namespace ns {
    void to_json(json& j, const Utils::ShaderParamValue& paramValue);
    void from_json(const json& j, Utils::ShaderParamValue& paramValue);
}

#endif
