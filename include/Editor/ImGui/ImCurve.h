#ifndef IMGUI_CURVE_H
#define IMGUI_CURVE_H

#include <string>
#include "imgui.h"
#include "Platform.h"
#include "Utils/Curve.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <cmath>

namespace ImGui
{
    bool CurveEditor(const std::string &label, const ImVec2& size, Curve &curve);
	bool CurveButton(const std::string &label, u32 size, Curve &curve);
};

namespace Tween {
    enum TYPE
    {
        LINEAR,

        QUADIN,          // t^2
        QUADOUT,
        QUADINOUT,
        CUBICIN,         // t^3
        CUBICOUT,
        CUBICINOUT,
        QUARTIN,         // t^4
        QUARTOUT,
        QUARTINOUT,
        QUINTIN,         // t^5
        QUINTOUT,
        QUINTINOUT,
        SINEIN,          // sin(t)
        SINEOUT,
        SINEINOUT,
        EXPOIN,          // 2^t
        EXPOOUT,
        EXPOINOUT,
        CIRCIN,          // sqrt(1-t^2)
        CIRCOUT,
        CIRCINOUT,
        ELASTICIN,       // exponentially decaying sine wave
        ELASTICOUT,
        ELASTICINOUT,
        BACKIN,          // overshooting cubic easing: (s+1)*t^3 - s*t^2
        BACKOUT,
        BACKINOUT,
        BOUNCEIN,        // exponentially decaying parabolic bounce
        BOUNCEOUT,
        BOUNCEINOUT,

        SINESQUARE,      // gapjumper's
        EXPONENTIAL,     // gapjumper's
        SCHUBRING1,      // terry schubring's formula 1
        SCHUBRING2,      // terry schubring's formula 2
        SCHUBRING3,      // terry schubring's formula 3

        SINPI2,          // tomas cepeda's
        SWING,           // tomas cepeda's & lquery's
    };
};

#endif