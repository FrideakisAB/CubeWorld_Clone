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

    unsigned int GetPrimitiveGL(Primitive prim) noexcept;
    unsigned int GetDrawTypeGL(DrawType dt) noexcept;
    unsigned int GetValueTypeGL(ValueType vt) noexcept;
}

#endif
