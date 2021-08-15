#ifndef CURVE_H
#define CURVE_H

#include "Platform.h"
#include <glm/glm.hpp>

namespace Curve {
    f32 CurveValue(f32 position, u8 maxPoints, const glm::vec2 *points);
};

#endif
