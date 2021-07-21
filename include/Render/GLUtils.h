#ifndef GLUTILS_H
#define GLUTILS_H

#include <GL/glew.h>
#include "Utils/glm.h"
#include "Render/Render.h"

namespace Utils {
    struct DirectionLight {
        glm::vec4 direction;
        glm::vec4 colorAndIntensity;
    };
    struct PointLight {
        glm::vec4 positionAndIntensity;
        glm::vec4 colorAndRadius;
    };
    struct SpotLight {
        glm::vec4 positionAndIntensity;
        glm::vec4 colorAndRadius;
        glm::vec4 directionAndCutterAngle;
    };

    constexpr unsigned int GetPrimitiveGL(Primitive prim) noexcept
    {
        unsigned int primGL = 0;
        switch(prim)
        {
        case Primitive::Triangles:
            primGL = GL_TRIANGLES;
            break;

        case Primitive::Lines:
            primGL = GL_LINES;
            break;

        case Primitive::Points:
            primGL = GL_POINTS;
            break;

        default:
            primGL = 0;
            break;
        }

        return primGL;
    }

    constexpr unsigned int GetDrawTypeGL(DrawType dt) noexcept
    {
        unsigned int dtGL = 0;
        switch(dt)
        {
        case DrawType::Static:
            dtGL = GL_STATIC_DRAW;
            break;

        case DrawType::Dynamic:
            dtGL = GL_DYNAMIC_DRAW;
            break;

        case DrawType::Stream:
            dtGL = GL_STREAM_DRAW;
            break;

        default:
            dtGL = 0;
            break;
        }

        return dtGL;
    }

    constexpr unsigned int GetValueTypeGL(ValueType vt) noexcept
    {
        unsigned int vtGL = 0;
        switch(vt)
        {
        case ValueType::Byte:
            vtGL = GL_BYTE;
            break;

        case ValueType::UByte:
            vtGL = GL_UNSIGNED_BYTE;
            break;

        case ValueType::Short:
            vtGL = GL_SHORT;
            break;

        case ValueType::UShort:
            vtGL = GL_UNSIGNED_SHORT;
            break;

        case ValueType::Int:
            vtGL = GL_INT;
            break;

        case ValueType::UInt:
            vtGL = GL_UNSIGNED_INT;
            break;

        case ValueType::HalfFloat:
            vtGL = GL_HALF_FLOAT;
            break;

        case ValueType::Float:
            vtGL = GL_FLOAT;
            break;

        case ValueType::Double:
            vtGL = GL_DOUBLE;
            break;

        default:
            vtGL = 0;
            break;
        }

        return vtGL;
    }

    constexpr unsigned int GetTextDataTypeGL(TexDataType tdt) noexcept
    {
        unsigned int ttGL = 0;
        switch(tdt)
        {
        case TexDataType::R:
            ttGL = GL_RED;
            break;

        case TexDataType::R16:
            ttGL = GL_ALPHA16;
            break;

        case TexDataType::RF:
            ttGL = GL_R32F;
            break;

        case TexDataType::R16F:
            ttGL = GL_R16F;
            break;

        case TexDataType::RG:
            ttGL = GL_RG;
            break;

        case TexDataType::RG16F:
            ttGL = GL_RG16F;
            break;

        case TexDataType::RG16:
            ttGL = GL_RG16;
            break;

        case TexDataType::RGB:
            ttGL = GL_RGB;
            break;

        case TexDataType::RGB16F:
            ttGL = GL_RGB16F;
            break;

        case TexDataType::RGB16:
            ttGL = GL_RGB16;
            break;

        case TexDataType::RGBA:
            ttGL = GL_RGBA;
            break;

        case TexDataType::RGBA16F:
            ttGL = GL_RGBA16F;
            break;

        case TexDataType::RGBA16:
            ttGL = GL_RGBA16;
            break;

        case TexDataType::RGBAF:
            ttGL = GL_RGBA32F;
            break;

        default:
            ttGL = 0;
            break;
        }

        return ttGL;
    }

    constexpr unsigned int GetWrapTypeGL(WrapType wt) noexcept
    {
        unsigned int wtGL = 0;
        switch(wt)
        {
        case WrapType::Repeat:
            wtGL = GL_REPEAT;
            break;

        case WrapType::MirroredRepeat:
            wtGL = GL_MIRRORED_REPEAT;
            break;

        case WrapType::ClampToEdge:
            wtGL = GL_CLAMP_TO_EDGE;
            break;

        case WrapType::ClampToBorder:
            wtGL = GL_CLAMP_TO_BORDER;
            break;

        default:
            wtGL = 0;
            break;
        }

        return wtGL;
    }

    constexpr unsigned int GetFilteringGL(Filtering fg) noexcept
    {
        unsigned int fgGL = 0;
        switch(fg)
        {
        case Filtering::Nearest:
            fgGL = GL_NEAREST;
            break;

        case Filtering::Linear:
            fgGL = GL_LINEAR;
            break;

        default:
            fgGL = 0;
            break;
        }

        return fgGL;
    }
}

#endif
