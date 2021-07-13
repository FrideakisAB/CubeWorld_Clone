#ifndef GLUTILS_H
#define GLUTILS_H

#include <GL/glew.h>
#include "Render/Render.h"

namespace Utils {
    unsigned int GetPrimitiveGL(Primitive prim) noexcept;
    unsigned int GetDrawTypeGL(DrawType dt) noexcept;
    unsigned int GetValueTypeGL(ValueType vt) noexcept;
}

#endif
