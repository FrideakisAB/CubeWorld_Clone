#include "Utils/Curve.h"

// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
// key format (for dim == 1) is (t0,x0,t1,x1 ...)
// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
void Spline(const f32 *key, size_t count, size_t dim, f32 position, f32 *output)
{
    static i8 coefs[16] = {
            -1, 2,-1, 0,
            3,-5, 0, 2,
            -3, 4, 1, 0,
            1,-1, 0, 0
    };

    const size_t size = dim + 1;

    size_t k = 0;
    while (key[k*size] < position)
        ++k;

    // interpolate
    const f32 h = (position - key[(k - 1) * size]) / (key[k * size] - key[(k - 1) * size]);

    // init result
    for (size_t i = 0; i < dim; ++i)
        output[i] = 0.0f;

    // add basis functions
    for (size_t i = 0; i < 4; ++i)
    {
        size_t kn = k + i - 2;
        if (k + i < 2)
            kn = 0;
        else if (kn > (count - 1))
            kn = count - 1;

        const i8 *co = coefs + 4 * i;

        const f32 b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

        for (size_t j = 0; j < dim; ++j)
            output[j] += b * key[kn * size + j + 1];
    }
}

Curve::Curve()
{
    points[0].x = 0;
    points[0].y = 0;
    points[1].x = 1;
    points[1].y = 1;
    points[2].x = -1;
}

Curve::Curve(const Curve &curve)
{
    std::copy(curve.points, curve.points + MaxCurvePoints, points);
}

Curve &Curve::operator=(const Curve &curve)
{
    std::copy(curve.points, curve.points + MaxCurvePoints, points);
    return *this;
}

f32 Curve::Value(f32 position) noexcept
{
    if (position < 0)
        return points[0].y;

    u32 left = 0;
    while (left < MaxCurvePoints && points[left].x < position && points[left].x != -1)
        left++;

    if (left)
        left--;

    if (left == MaxCurvePoints - 1)
        return points[MaxCurvePoints - 1].y;

    f32 delta = (position - points[left].x) / (points[left + 1].x - points[left].x);

    return points[left].y + (points[left + 1].y - points[left].y) * delta;
}

f32 Curve::ValueSmooth(f32 position) noexcept
{
    if (position < 0)
        return points[0].y;

    auto *input = new f32[MaxCurvePoints * 2];
    f32 output[4];

    for (size_t i = 0; i < MaxCurvePoints; ++i)
    {
        input[i * 2 + 0] = points[i].x;
        input[i * 2 + 1] = points[i].y;
    }

    Spline(input, MaxCurvePoints, 1, position, output);

    delete[] input;
    return output[0];
}

json Curve::SerializeObj() const
{
    json j;
    for (size_t i = 0; i < MaxCurvePoints; ++i)
        j[i] = {points[i].x, points[i].y};

    return j;
}

void Curve::UnSerializeObj(const json &j)
{
    for(size_t i = 0; i < MaxCurvePoints; ++i)
    {
        points[i].x = j[i][0];
        points[i].y = j[i][1];
    }
}
