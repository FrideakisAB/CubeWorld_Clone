#ifndef CAMERA_H
#define CAMERA_H

#include "ECS/ECS.h"
#include "Utils/glm.h"
#include "Render/Texture.h"
#include "Assets/AssetsManager.h"

enum class Projection {
    Perspective = 0,
    Orthographic
};

class Camera : public ECS::Component<Camera> {
private:
    AssetsHandle skyboxHandle;
    Texture *skybox = nullptr;

public:
    static Camera* Main;
    float Fov = 60.0f;
    float Ratio = 1.5f;
    float NearClip = 0.1f;
    float FarClip = 100.0f;
    Projection Proj = Projection::Perspective;

    [[nodiscard]] glm::mat4 GetVPMatrix(u32 width, u32 height) const;
    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    [[nodiscard]] glm::mat4 GetProjMatrix(u32 width, u32 height) const;

    [[nodiscard]] bool IsValidSkybox() const noexcept { return skybox != nullptr; }

    void SetSkybox(Texture *skybox);
    void SetSkybox(const AssetsHandle &skybox);

    inline AssetsHandle& GetAsset() { return skyboxHandle; }
    [[nodiscard]] Texture &GetSkybox() const noexcept { return *skybox; }
};

#endif
