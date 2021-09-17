#ifndef CURVE_H
#define CURVE_H

#include "Platform.h"
#include <glm/glm.hpp>
#include "Assets/ISerialize.h"

constexpr size_t MaxCurvePoints = 10;

class Curve final : public ISerialize {
private:
    glm::vec2 points[MaxCurvePoints]{};

public:
    Curve();
    Curve(const Curve &curve);

    Curve &operator=(const Curve &curve);

    f32 Value(f32 position) noexcept;
    f32 ValueSmooth(f32 position) noexcept;

    [[nodiscard]] glm::vec2 *GetPoints() noexcept { return points; }
    [[nodiscard]] static size_t GetPointsCount() noexcept { return MaxCurvePoints; }

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json &j) final;
};

#endif
