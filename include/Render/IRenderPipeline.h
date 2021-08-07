#ifndef IRENDERPIPELINE_H
#define IRENDERPIPELINE_H

class IRenderPipeline {
public:
    virtual void ApplyTasks(std::map<std::string, MaterialMap>& renderObjects, std::vector<RenderTask>& renderTasks) = 0;
    virtual void ApplyMaterials(std::unordered_map<Material*, MaterialSet>& materialTranslation) = 0;
    virtual void ApplyLights(DirectionLight directionLight, SSBO<PointLight>& pointLights, u16 pointLightPos, SSBO<SpotLight>& spotLights, u16 spotLightPos) = 0;

    virtual void Render() = 0;
};

#endif
