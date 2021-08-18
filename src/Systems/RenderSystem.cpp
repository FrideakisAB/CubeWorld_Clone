#include "Systems/RenderSystem.h"

#include <fstream>
#include <filesystem>
#include "Render/Texture.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/LightSource.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystem.h"
#include "Render/ForwardPlusPipeline.h"

namespace fs = std::filesystem;

RenderSystem::RenderSystem()
{
    Utils::SetupDebugOpenGL();
    renderPipeline = new ForwardPlusPipeline();

    for (auto &entry : fs::recursive_directory_iterator("data\\shaders"))
    {
        std::error_code ec;
        if (!entry.is_directory(ec) && !ec && entry.path().extension() == ".json")
        {
            json j = json_utils::TryParse(Utils::FileToString(std::ifstream(entry.path())));

            if (j["tag"] == "SHADER")
                shaders[j["shaderName"]].UnSerializeObj(j);
        }
    }

    renderPipeline->ApplyShaders(shaders);
    renderPipeline->ApplyLightSources(pointLightSources, spotLightSources);

    pointLights.UploadData(1);
    pointLightCount = 1;
    spotLights.UploadData(1);
    spotLightCount = 1;
}

RenderSystem::~RenderSystem()
{
    delete renderPipeline;
}

void RenderSystem::PreUpdate()
{
    auto *CM = ECS::ECS_Engine->GetComponentManager();
    auto *EM = ECS::ECS_Engine->GetEntityManager();

    pointLightSources.reserve(pointLightCount);
    spotLightSources.reserve(spotLightCount);
    for (auto& lightSource : CM->GetIterator<LightSource>())
    {
        auto *entity = EM->GetEntity(lightSource.GetOwner());
        if (entity != nullptr && entity->IsActive() && lightSource.IsActive())
        {
            auto *transform = entity->GetComponent<Transform>();
            if (transform != nullptr && transform->IsActive())
            {
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
                    light.directionAndCutterAngle = glm::vec4(direction, glm::radians(lightSource.GetCutterOff()));

                    spotLightSources.emplace_back(light);
                }
                else
                {
                    Utils::DirectionLight light;
                    auto position = glm::vec3(transform->GetMat()[3]);
                    light.position = glm::vec4(position, 1.0f);
                    light.colorAndIntensity = glm::vec4(lightSource.GetColor(), lightSource.GetIntensity());
                    glm::vec3 direction = transform->GetGlobalPos().rotate * glm::vec3(0.0f, -1.0f, 0.0f);
                    light.direction = glm::vec4(direction, 1.0f);

                    directionLight = light;
                }
            }
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

    RenderMask Shadows; Shadows.NoShadows = false;
    RenderMask NoShadows; NoShadows.NoShadows = true;

    for (auto& mesh : CM->GetIterator<MeshComponent>())
    {
        auto *entity = EM->GetEntity(mesh.GetOwner());
        if (entity != nullptr && entity->IsActive() && mesh.IsActive() && mesh.IsValid())
        {
            auto *materialComponent = entity->GetComponent<MaterialComponent>();
            auto *transform = entity->GetComponent<Transform>();
            if (materialComponent != nullptr && materialComponent->IsActive() && transform != nullptr && transform->IsActive())
            {
                mesh.GetMesh().RenderUpdate();

                Material *material = materialComponent->GetMaterial();

                if (materialTranslation.find(material) == materialTranslation.end())
                    importMaterial(material);

                DrawData data = mesh.GetMesh().GetDrawData();

                if (data.VAO != 0)
                {
                    renderTasks.push_back({data, Shadows, transform->GetMat()});
                    renderObjects[material->Shader][material].push_back(renderTasks.size() - 1);
                }
            }
        }
    }

    for (auto& ps : CM->GetIterator<ParticleSystem>())
    {
        auto *entity = EM->GetEntity(ps.GetOwner());
        if (entity != nullptr && entity->IsActive() && ps.IsActive())
        {
            auto *materialComponent = entity->GetComponent<MaterialComponent>();
            auto *transform = entity->GetComponent<Transform>();
            if (materialComponent != nullptr && materialComponent->IsActive() && transform != nullptr && transform->IsActive())
            {
                //Remove it
                ps.Update();

                ps.GetRender().RenderUpdate();

                Material *material = materialComponent->GetMaterial();

                if (materialTranslation.find(material) == materialTranslation.end())
                    importMaterial(material);

                DrawData data = ps.GetRender().GetDrawData();

                if (data.VAO != 0)
                {
                    if (!ps.GlobalSpace)
                        renderTasks.push_back({data, NoShadows, transform->GetMat()});
                    else
                        renderTasks.push_back({data, NoShadows, glm::mat4(1.0f)});
                    renderObjects[material->Shader][material].push_back(renderTasks.size() - 1);
                }
            }
        }
    }
}

void RenderSystem::Update()
{
    if (Camera::Main != nullptr)
    {
        renderPipeline->ApplyLights(directionLight, pointLights, pointLightPos, spotLights, spotLightPos);
        renderPipeline->ApplyMaterials(materialTranslation);
        renderPipeline->ApplyTasks(renderObjects, renderTasks);

        if (sizeUpdate)
        {
            renderPipeline->Resize(offsetX, offsetY, width, height);
            sizeUpdate = false;
        }

        renderPipeline->Render();
    }
}

void RenderSystem::PostUpdate()
{
    renderPipeline->Release();
    renderObjects.clear();
    renderTasks.clear();
    materialTranslation.clear();
    pointLightSources.clear();
    spotLightSources.clear();
}

void RenderSystem::importMaterial(Material *material)
{
    MaterialSet materialSet{};

    materialSet.Uniforms = material->Uniforms;

    for (const auto &[name, texture] : material->Samplers)
        if (auto *texPtr = dynamic_cast<Texture *>(texture.get()))
        {
            texPtr->RenderUpdate();
            materialSet.Samplers[name] = texPtr->GetDrawData();
        }

    materialTranslation[material] = std::move(materialSet);
}

void RenderSystem::Resize(u16 offsetX, u16 offsetY, u16 width, u16 height) noexcept
{
    if (this->offsetX != offsetX)
    {
        this->offsetX = offsetX;
        sizeUpdate = true;
    }

    if (this->offsetY != offsetY)
    {
        this->offsetY = offsetY;
        sizeUpdate = true;
    }

    if (this->width != width)
    {
        this->width = width;
        sizeUpdate = true;
    }

    if (this->height != height)
    {
        this->height = height;
        sizeUpdate = true;
    }
}
