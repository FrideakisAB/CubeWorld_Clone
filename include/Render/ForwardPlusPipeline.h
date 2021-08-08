#ifndef FORWARDPLUSPIPELINE_H
#define FORWARDPLUSPIPELINE_H

#include "Render/IRenderPipeline.h"

class ForwardPlusPipeline : public IRenderPipeline<ForwardPlusPipeline> {
public:
    ForwardPlusPipeline();
    ~ForwardPlusPipeline();

    void ApplyTasks(std::map<std::string, RenderSystem::MaterialMap> &renderObjects, std::vector<RenderSystem::RenderTask> &renderTasks);
    void ApplyMaterials(std::unordered_map<Material*, RenderSystem::MaterialSet> &materialTranslation);
    void ApplyLights(std::optional<DirectionLight> directionLight, SSBO<PointLight> &pointLights, u16 pointLightPos, SSBO<SpotLight> &spotLights, u16 spotLightPos);

    void Resize(u16 offsetX, u16 offsetY, u16 width, u16 height);
    void Render();
    void Release();
};

#endif
