#ifndef CAMERA_H
#define CAMERA_H

#include "ECS/ECS.h"
#include "Utils/glm.h"

enum class Projection {
    Perspective = 0,
    Orthographic
};

class Camera : public ECS::Component<Camera> {
public:
    static Camera* Main;
    float Fov = 45.0f;
    float Ratio = 1.5f;
    float NearClip = 0.1f;
    float FarClip = 100.0f;
    Projection Proj = Projection::Perspective;

    [[nodiscard]] glm::mat4 GetVPMatrix(u32 width, u32 height) const;
    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    [[nodiscard]] glm::mat4 GetProjMatrix(u32 width, u32 height) const;
};

#endif
