#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <map>
#include <vector>
#include "ECS/ECS.h"
#include <functional>
#include "Render/SSBO.h"
#include "Render/Shader.h"
#include "Render/GLUtils.h"
#include "Utils/ShaderPackager.h"
#include "Components/MaterialComponent.h"

class RenderSystem : public ECS::System<RenderSystem> {
    struct MaterialSet {
        std::map<std::string, Utils::ShaderParamValue> Uniforms;
        std::map<std::string, SamplerData> Samplers;
    };

    using RenderTask = DrawData;
    using MaterialMap = std::map<Material*, std::vector<RenderTask>>;

private:
    std::unordered_map<Material*, MaterialSet> materialTranslation;
    std::map<std::string, MaterialMap> renderObjects;

    std::unordered_map<std::string, Shader> shaders;
    std::vector<std::function<void()>> deletedHandles;
    Utils::DirectionLight directionLight;
    SSBO<Utils::PointLight> pointLights;
    SSBO<Utils::SpotLight> spotLights;
    u16 pointLightCount = 0;
    u16 pointLightPos = 0;
    u16 spotLightCount = 0;
    u16 spotLightPos = 0;

    void importMaterial(Material *material);

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
