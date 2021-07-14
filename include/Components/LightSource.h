#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "Utils/glm.h"
#include "ECS/ECS.h"

enum class LightType : u8 {
    Directional = 0,
    Point,
    Spot
};

class LightSource : public ECS::Component<LightSource> {
private:
    LightType type{};
    glm::vec3 color = glm::vec3(1.0f);
    float radius = 1.0f;
    float intensity = 1.0f;
    float cuttOff = 0.0f;
    bool update = true;

public:
    [[nodiscard]] LightType GetLightType() const noexcept { return type; }
    void SetLightType(LightType lightType);
    [[nodiscard]] glm::vec3 GetColor() const noexcept { return color; }
    void SetColor(glm::vec3 color);
    [[nodiscard]] float GetRadius() const noexcept { return radius; }
    void SetRadius(float radius);
    [[nodiscard]] float GetIntensity() const noexcept { return intensity; }
    void SetIntensity(float intensity);
    [[nodiscard]] float GetCutterOff() const noexcept { return cuttOff; }
    void SetCutterOff(float cutterOff);

    inline void ReleaseUpdate() { update = false; }
    [[nodiscard]] bool IsUpdate() const noexcept { return update; }
};

#endif
