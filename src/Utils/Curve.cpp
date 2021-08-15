#include "utils/Curve.h"

f32 Curve::CurveValue(f32 position, u8 maxPoints, const glm::vec2 *points)
{
    if(maxPoints >= 2 && points != nullptr)
    {
        if (position < 0)
            return points[0].y;

        u8 left = 0;
        while(left < maxPoints && points[left].x < position && points[left].x != -1)
            left++;

        if(left)
            left--;

        if(left == maxPoints - 1)
            return points[maxPoints - 1].y;

        f32 d = (position - points[left].x) / (points[left + 1].x - points[left].x);

        return points[left].y + (points[left + 1].y - points[left].y) * d;
    }

    return 0;
}