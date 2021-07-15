#include "Systems/RenderSystem.h"

#include "Components/Transform.h"
#include "Components/LightSource.h"

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{

}

void RenderSystem::PreUpdate()
{
    auto *CM = ECS::ECS_Engine->GetComponentManager();
    auto *EM = ECS::ECS_Engine->GetEntityManager();

    std::vector<Utils::PointLight> pointLightSources;
    pointLightSources.reserve(pointLightCount);
    std::vector<Utils::SpotLight> spotLightSources;
    spotLightSources.reserve(spotLightCount);
    for(auto& lightSource : CM->GetIterator<LightSource>())
    {
        auto *entity = EM->GetEntity(lightSource.GetOwner());
        if(entity != nullptr && entity->IsActive() && lightSource.IsActive())
        {
            auto *transform = entity->GetComponent<Transform>();
            if (lightSource.GetLightType() == LightType::Point)
            {
                Utils::PointLight light;
                light.colorAndRadius = glm::vec4(lightSource.GetColor(), lightSource.GetRadius());
                auto position = glm::vec3(transform->GetMat()[3]);
                light.positionAndIntensity = glm::vec4(position, lightSource.GetIntensity());

                pointLightSources.emplace_back(light);
            }
            else if (lightSource.GetLightType() == LightType::Spot)
            {
                Utils::SpotLight light;
                light.colorAndRadius = glm::vec4(lightSource.GetColor(), lightSource.GetRadius());
                auto position = glm::vec3(transform->GetMat()[3]);
                light.positionAndIntensity = glm::vec4(position, lightSource.GetIntensity());
                glm::vec3 direction = transform->GetGlobalPos().rotate * glm::vec3(0.0f, -1.0f, 0.0f);
                light.directionAndCutterAngle = glm::vec4(direction, lightSource.GetCutterOff());

                spotLightSources.emplace_back(light);
            }
            else
            {
                directionLight.colorAndIntensity = glm::vec4(lightSource.GetColor(), lightSource.GetIntensity());
                glm::vec3 direction = transform->GetGlobalPos().rotate * glm::vec3(0.0f, -1.0f, 0.0f);
                directionLight.direction = glm::vec4(direction, 1.0f);
            }

            lightSource.ReleaseUpdate();
        }
    }

    if (pointLightCount >= pointLightSources.size())
    {
        pointLights.UpdateData(pointLightSources.size(), pointLightSources.data());
        pointLightPos = static_cast<u16>(pointLightSources.size());
    }
    else
    {
        pointLightCount = static_cast<u16>(pointLightSources.size());
        pointLightPos = static_cast<u16>(pointLightSources.size());
        pointLights.UploadData(pointLightCount, pointLightSources.data());
    }

    if (spotLightCount >= spotLightSources.size())
    {
        spotLights.UpdateData(spotLightSources.size(), spotLightSources.data());
        spotLightPos = static_cast<u16>(spotLightSources.size());
    }
    else
    {
        spotLightCount = static_cast<u16>(spotLightSources.size());
        spotLightPos = static_cast<u16>(spotLightSources.size());
        spotLights.UploadData(spotLightCount, spotLightSources.data());
    }
}

void RenderSystem::Update()
{

}

void RenderSystem::PostUpdate()
{

}
