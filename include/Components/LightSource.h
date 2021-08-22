#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "ECS/ECS.h"
#include "Utils/glm.h"

enum class LightType : u8 {
    Directional = 0,
    Point,
    Spot
};

class LightSource final : public ECS::Component<LightSource> {
public:
    LightType Type = LightType::Directional;
    glm::vec3 Color = glm::vec3(1.0f);
    float Radius = 1.0f;
    float Intensity = 1.0f;
    float CutterOff = 0.0f;

    [[nodiscard]] json SerializeObj() final;
    void UnSerializeObj(const json &j) final;
};

#endif
