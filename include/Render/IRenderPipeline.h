#ifndef IRENDERPIPELINE_H
#define IRENDERPIPELINE_H

#include "Systems/RenderSystem.h"

using namespace Utils;

template<typename Pipeline>
class IRenderPipeline {
public:
    void ApplyTasks(std::map<std::string, RenderSystem::MaterialMap> &renderObjects, std::vector<RenderSystem::RenderTask> &renderTasks)
    {
        static_cast<Pipeline*>(this)->ApplyTasks(renderObjects, renderTasks);
    }

    void ApplyMaterials(std::unordered_map<Material*, RenderSystem::MaterialSet> &materialTranslation)
    {
        static_cast<Pipeline*>(this)->ApplyMaterials(materialTranslation);
    }

    void ApplyLights(std::optional<DirectionLight> directionLight, SSBO<PointLight> &pointLights, u16 pointLightPos, SSBO<SpotLight> &spotLights, u16 spotLightPos)
    {
        static_cast<Pipeline*>(this)->ApplyLights(directionLight, pointLights, pointLightPos, spotLights, spotLightPos);
    }

    void Resize(u16 offsetX, u16 offsetY, u16 width, u16 height)
    {
        static_cast<Pipeline*>(this)->Resize(offsetX, offsetY, width, height);
    }

    void Render()
    {
        static_cast<Pipeline*>(this)->Render();
    }

    void Release()
    {
        static_cast<Pipeline*>(this)->Release();
    }
};

#endif
