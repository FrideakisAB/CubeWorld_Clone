#include "Utils/Curve.h"

// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
// key format (for dim == 1) is (t0,x0,t1,x1 ...)
// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
void Curve::Spline(const float *key, int num, int dim, float t, float *v)
{
    static signed char coefs[16] = {
            -1, 2,-1, 0,
            3,-5, 0, 2,
            -3, 4, 1, 0,
            1,-1, 0, 0
    };

    const int size = dim + 1;

    int k = 0;
    while(key[k*size] < t)
        ++k;

    // interpolant
    const float h = (t - key[(k-1)*size]) / (key[k*size] - key[(k-1)*size]);

    // init result
    for(int i = 0; i < dim; ++i)
        v[i] = 0.0f;

    // add basis functions
    for(int i = 0; i < 4; ++i)
    {
        int kn = k + i - 2;
        if(kn < 0)
            kn = 0;
        else if(kn > (num - 1))
            kn = num - 1;

        const signed char* co = coefs + 4 * i;

        const float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

        for(int j = 0; j < dim; ++j)
            v[j] += b * key[kn * size + j + 1];
    }
}

f32 Curve::CurveValue(f32 position, u32 maxPoints, const glm::vec2 *points)
{
    if(maxPoints >= 2 && points != nullptr)
    {
        if (position < 0)
            return points[0].y;

        u32 left = 0;
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

f32 Curve::CurveValueSmooth(f32 position, u32 maxPoints, const glm::vec2 *points)
{
    if (maxPoints >= 2 && points != nullptr)
    {
        if (position < 0)
            return points[0].y;

        auto *input = new f32[maxPoints * 2];
        float output[4];

        for (u32 i = 0; i < maxPoints; ++i)
        {
            input[i * 2 + 0] = points[i].x;
            input[i * 2 + 1] = points[i].y;
        }

        Curve::Spline(input, static_cast<int>(maxPoints), 1, position, output);

        delete[] input;
        return output[0];
    }

    return 0;
}
