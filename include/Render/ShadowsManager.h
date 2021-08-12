#ifndef SHADOWSMANAGER_H
#define SHADOWSMANAGER_H

#include "Render/IRenderPipeline.h"

class ShadowsManager {
private:
    glm::mat4 dirLightVP;

    f32 shadowDrawDistance = 1024.0f;
    f32 dirLightDrawDistance = 50.0f;

    u32 dirDepthMapFBO, dirDepthMap;

    u16 dirLightShadowResolution = 4096;
    u16 spotLightShadowResolution = 2048;
    u16 pointLightShadowResolution = 1024;

    u8 isPointShadowsActive         : 1;
    u8 isSpotShadowsActive          : 1;
    u8 isDirectionalShadowsActive   : 1;
    u8 isShadowsActive              : 1;
    u8 isDeleteDirClear             : 1;
    u8 reserve                      : 3;

    std::optional<DirectionLight> directionLight;
    std::vector<Utils::PointLight> *pointLightSources = nullptr;
    std::vector<Utils::SpotLight> *spotLightSources = nullptr;

public:
    ShadowsManager();
    ~ShadowsManager();

    void ApplyLightSources(std::optional<DirectionLight> directionLight, std::vector<Utils::PointLight> &pointLightSources, std::vector<Utils::SpotLight> &spotLightSources);

    void Render(glm::vec3 cameraPosition, std::unordered_map<std::string, Shader> &shaders, std::vector<RenderSystem::RenderTask> &renderTasks);
    [[nodiscard]] u32 GetDirectionDepthMap() const noexcept { return dirDepthMap; }
    [[nodiscard]] glm::mat4 GetDirectionVP() const noexcept { return dirLightVP; }
};

#endif
