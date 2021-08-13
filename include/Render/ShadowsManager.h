#ifndef SHADOWSMANAGER_H
#define SHADOWSMANAGER_H

#include "Render/IRenderPipeline.h"

struct SpotShadowData {
    glm::vec4 positionAndFarPlane;
    glm::vec4 directionAndCutterAngle;
    glm::mat4 vp;
};

class ShadowsManager {
private:
    glm::mat4 dirLightVP = glm::mat4(1.0f);

    f32 shadowDrawDistance = 1024.0f;
    f32 dirLightDrawDistance = 50.0f;

    u32 dirDepthMapFBO = 0, dirDepthMap = 0;
    u32 pointDepthMapFBO[3]{};
    u32 spotDepthMapFBO[3]{};

    SSBO<glm::vec4> pointLightPositions;
    glm::uvec3 pointShadowCount = glm::uvec3(0);
    u32 pointDepthMaps[3]{};

    SSBO<SpotShadowData> spotLightData;
    glm::uvec3 spotShadowCount = glm::uvec3(0);
    u32 spotDepthMaps[3]{};

    static constexpr u16 pointLightHighShadowCount = 4;    // 72Mb
    static constexpr u16 pointLightMediumShadowCount = 8; //  36Mb
    static constexpr u16 pointLightLowShadowCount = 48;  //   54Mb

    static constexpr u16 spotLightHighShadowCount = 4;    // 48Mb
    static constexpr u16 spotLightMediumShadowCount = 8; //  24Mb
    static constexpr u16 spotLightLowShadowCount = 48;  //   36Mb

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

    [[nodiscard]] u32 GetPointHighDepthMap() const noexcept { return pointDepthMaps[0]; }
    [[nodiscard]] u32 GetPointMediumDepthMap() const noexcept { return pointDepthMaps[1]; }
    [[nodiscard]] u32 GetPointLowDepthMap() const noexcept { return pointDepthMaps[2]; }
    [[nodiscard]] glm::uvec3 GetPointCount() const noexcept { return pointShadowCount; }
    [[nodiscard]] SSBO<glm::vec4> &GetPointPositions() noexcept { return pointLightPositions; }

    [[nodiscard]] u32 GetSpotHighDepthMap() const noexcept { return spotDepthMaps[0]; }
    [[nodiscard]] u32 GetSpotMediumDepthMap() const noexcept { return spotDepthMaps[1]; }
    [[nodiscard]] u32 GetSpotLowDepthMap() const noexcept { return spotDepthMaps[2]; }
    [[nodiscard]] glm::uvec3 GetSpotCount() const noexcept { return spotShadowCount; }
    [[nodiscard]] SSBO<SpotShadowData> &GetSpotShadowData() noexcept { return spotLightData; }

    void AttachShadowsData() noexcept;
    void SetUniforms(Shader &shader) const noexcept;
};

#endif
