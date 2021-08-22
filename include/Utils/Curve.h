#ifndef CURVE_H
#define CURVE_H

#include "Platform.h"
#include <glm/glm.hpp>

namespace Curve {
    void Spline(const float *key, int num, int dim, float t, float *v);
    f32 CurveValue(f32 position, u8 maxPoints, const glm::vec2 *points);
    f32 CurveValueSmooth(f32 position, i32 maxPoints, const glm::vec2 *points);
};

#endif
