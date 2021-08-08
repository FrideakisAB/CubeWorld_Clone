#include "Render/ForwardPlusPipeline.h"

ForwardPlusPipeline::ForwardPlusPipeline()
{

}

ForwardPlusPipeline::~ForwardPlusPipeline()
{

}

void ForwardPlusPipeline::ApplyTasks(std::map<std::string, RenderSystem::MaterialMap> &renderObjects, std::vector<RenderSystem::RenderTask> &renderTasks)
{

}

void ForwardPlusPipeline::ApplyMaterials(std::unordered_map<Material*, RenderSystem::MaterialSet> &materialTranslation)
{

}

void ForwardPlusPipeline::ApplyLights(std::optional<DirectionLight> directionLight, SSBO<PointLight> &pointLights, u16 pointLightPos, SSBO<SpotLight> &spotLights, u16 spotLightPos)
{

}

void ForwardPlusPipeline::Resize(u16 offsetX, u16 offsetY, u16 width, u16 height)
{

}

void ForwardPlusPipeline::Render()
{

}

void ForwardPlusPipeline::Release()
{

}
