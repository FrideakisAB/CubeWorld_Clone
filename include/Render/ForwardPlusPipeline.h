#ifndef FORWARDPLUSPIPELINE_H
#define FORWARDPLUSPIPELINE_H

#include "Render/ShadowsManager.h"
#include "Render/IRenderPipeline.h"

class ForwardPlusPipeline final : public IRenderPipeline<ForwardPlusPipeline> {
private:
    u32 depthMapFBO, depthMap;
    u32 skyboxVAO, skyboxVBO;
    u32 hdrFBO, rboDepth, colorBuffers[2];
    u32 pingPongFBO[2];
    u32 pingPongBuffers[2];
    u32 blurAmount = 10;

    ShadowsManager shadowsManager;

    u16 offsetX = 0, offsetY = 0;
    u16 width = 0, height = 0;

    std::map<std::string, RenderSystem::MaterialMap> *renderObjects;
    std::vector<RenderSystem::RenderTask> *renderTasks;
    std::unordered_map<Material*, RenderSystem::MaterialSet> *materialTranslation;

    std::optional<DirectionLight> directionLight;
    std::unordered_map<std::string, Shader> *shaders;
    SSBO<PointLight> *pointLights;
    SSBO<SpotLight> *spotLights;
    std::vector<Utils::PointLight> *pointLightSources;
    std::vector<Utils::SpotLight> *spotLightSources;
    SSBO<int> pointIndices;
    SSBO<int> spotIndices;
    u16 pointLightPos, spotLightPos;
    CameraInfo cameraInfo;

    void setupMaterial(Shader &shader, Material *material, u8 lastTexture);

public:
    ForwardPlusPipeline();
    ~ForwardPlusPipeline();

    void ApplyTasks(std::map<std::string, RenderSystem::MaterialMap> &renderObjects, std::vector<RenderSystem::RenderTask> &renderTasks);
    void ApplyMaterials(std::unordered_map<Material*, RenderSystem::MaterialSet> &materialTranslation);
    void ApplyShaders(std::unordered_map<std::string, Shader> &shaders);
    void ApplyLights(std::optional<DirectionLight> directionLight, SSBO<PointLight> &pointLights, u16 pointLightPos, SSBO<SpotLight> &spotLights, u16 spotLightPos);
    void ApplyLightSources(std::vector<Utils::PointLight> &pointLightSources, std::vector<Utils::SpotLight> &spotLightSources);
    void ApplyCamera(CameraInfo cameraInfo);

    void Resize(u16 offsetX, u16 offsetY, u16 width, u16 height);
    void Render();
    void Release();
};

#endif
