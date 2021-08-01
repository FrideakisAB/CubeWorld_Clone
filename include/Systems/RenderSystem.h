#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <map>
#include <vector>
#include "ECS/ECS.h"
#include <functional>
#include "Render/SSBO.h"
#include "Render/Shader.h"
#include "Render/GLUtils.h"

class RenderSystem : public ECS::System<RenderSystem> {
private:
    std::unordered_map<std::string, Shader> shaders;
    std::vector<std::function<void()>> deletedHandles;
    Utils::DirectionLight directionLight;
    SSBO<Utils::PointLight> pointLights;
    SSBO<Utils::SpotLight> spotLights;
    u16 pointLightCount = 0;
    u16 pointLightPos = 0;
    u16 spotLightCount = 0;
    u16 spotLightPos = 0;

public:
    RenderSystem();
    ~RenderSystem() override;

    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;

    void ReleaseHandle(const std::function<void()> &deleter)
    {
        deletedHandles.emplace_back(deleter);
    }
};

#endif
