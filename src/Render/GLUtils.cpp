#include "Render/GLUtils.h"

namespace Utils {
    unsigned int GetPrimitiveGL(Primitive prim) noexcept
    {
        unsigned int primGL;
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

    unsigned int GetDrawTypeGL(DrawType dt) noexcept
    {
        unsigned int dtGL;
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

    unsigned int GetValueTypeGL(ValueType vt) noexcept
    {
        unsigned int vtGL;
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
}