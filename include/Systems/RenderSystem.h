#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <map>
#include <vector>
#include <optional>
#include "ECS/ECS.h"
#include <functional>
#include "Render/SSBO.h"
#include "Render/Shader.h"
#include "Render/GLUtils.h"
#include "Utils/ShaderPackager.h"
#include "Components/MaterialComponent.h"

class ForwardPlusPipeline;

class RenderSystem : public ECS::System<RenderSystem> {
public:
    struct MaterialSet {
        std::map<std::string, Utils::ShaderParamValue> Uniforms;
        std::map<std::string, SamplerData> Samplers;
    };

    struct RenderTask {
        DrawData DrawData;
        glm::mat4 Transform;
    };

    using MaterialMap = std::map<Material*, std::vector<size_t>>;

private:
    std::unordered_map<Material*, MaterialSet> materialTranslation;
    std::map<std::string, MaterialMap> renderObjects;
    std::vector<RenderTask> renderTasks;

    ForwardPlusPipeline* renderPipeline;

    std::unordered_map<std::string, Shader> shaders;
    std::vector<std::function<void()>> deletedHandles;
    std::optional<Utils::DirectionLight> directionLight;
    SSBO<Utils::PointLight> pointLights;
    SSBO<Utils::SpotLight> spotLights;
    std::vector<Utils::PointLight> pointLightSources;
    std::vector<Utils::SpotLight> spotLightSources;
    u16 pointLightCount = 0;
    u16 pointLightPos = 0;
    u16 spotLightCount = 0;
    u16 spotLightPos = 0;

    u16 offsetX = 0, offsetY = 0;
    u16 width = 0, height = 0;
    bool sizeUpdate = true;

    void importMaterial(Material *material);

public:
    RenderSystem();
    ~RenderSystem() override;

    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;

    void Resize(u16 offsetX, u16 offsetY, u16 width, u16 height) noexcept;

    void ReleaseHandle(const std::function<void()> &deleter)
    {
        deletedHandles.emplace_back(deleter);
    }
};

#endif
