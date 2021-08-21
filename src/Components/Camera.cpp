#include "Components/Camera.h"

#include "Engine.h"
#include "Components/Transform.h"
#include <glm/gtx/matrix_decompose.hpp>

Camera* Camera::Main = nullptr;

glm::mat4 Camera::GetVPMatrix(u32 width, u32 height) const
{
    glm::vec3 sizeScale;
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;

    auto *transform = ECS::ECS_Engine->GetComponentManager()->GetComponent<Transform>(owner);
    glm::decompose(transform->GetMat(), sizeScale, orientation, position, skew, perspective);

    glm::mat4 projection;

    if(width == 0 || height == 0)
    {
        width = 1;
        height = 1;
    }

    if(Proj == Projection::Perspective)
        projection = glm::perspective(glm::radians(Fov), (float)width/(float)height, NearClip, FarClip);
    else
        projection = glm::ortho(0.0f, Ratio * (float)width/(float)height, 0.0f, Ratio * (float)height/(float)height, NearClip, FarClip);

    glm::vec3 up = orientation * Transform::WorldUp;
    glm::vec3 forward = orientation * Transform::WorldFront;
    glm::mat4 view = glm::lookAt(position, position + forward, up);

    return projection * view;
}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 sizeScale;
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;

    auto *transform = ECS::ECS_Engine->GetComponentManager()->GetComponent<Transform>(owner);
    glm::decompose(transform->GetMat(), sizeScale, orientation, position, skew, perspective);

    glm::vec3 up = orientation * Transform::WorldUp;
    glm::vec3 forward = orientation * Transform::WorldFront;
    glm::mat4 view = glm::lookAt(position, position + forward, up);

    return view;
}

glm::mat4 Camera::GetProjMatrix(u32 width, u32 height) const
{
    if(width == 0 || height == 0)
    {
        width = 1;
        height = 1;
    }

    if(Proj == Projection::Perspective)
        return glm::perspective(glm::radians(Fov), (float)width/(float)height, NearClip, FarClip);
    else
        return glm::ortho(0.0f, Ratio * (float)width/(float)height, 0.0f, Ratio * (float)height/(float)height, NearClip, FarClip);
}

void Camera::SetSkybox(const AssetsHandle &skybox)
{
    if(auto *skyboxPtr = dynamic_cast<Texture*>(skybox.get()))
    {
        skyboxHandle = skybox;
        this->skybox = skyboxPtr;
    }
    else
    {
        skyboxHandle = {};
        this->skybox = nullptr;
    }
}

void Camera::SetSkybox(Texture *skybox)
{
    if (skybox != nullptr)
    {
        skyboxHandle = AssetsHandle(skybox);
        this->skybox = skybox;
    }
    else
    {
        skyboxHandle = {};
        this->skybox = nullptr;
    }
}

json Camera::SerializeObj()
{
    json data;

    data["cmpName"] = boost::typeindex::type_id<Camera>().pretty_name();

    data["fov"] = Fov;
    data["ratio"] = Ratio;
    data["nearClip"] = NearClip;
    data["farClip"] = FarClip;
    data["proj"] = static_cast<u8>(Proj);
    data["skybox"] = skyboxHandle->GetName();

    return data;
}

void Camera::UnSerializeObj(const json &j)
{
    Fov = j["fov"];
    Ratio = j["ratio"];
    NearClip = j["nearClip"];
    FarClip = j["farClip"];
    Proj = static_cast<Projection>(j["proj"]);

    if (j["skybox"] != "")
    {
        AssetsHandle asset = GameEngine->GetAssetsManager().GetAsset(j["skybox"]);
        if(auto *skyboxPtr = dynamic_cast<Texture*>(asset.get()))
        {
            skyboxHandle = std::move(asset);
            skybox = skyboxPtr;
        }
        else
        {
            skyboxHandle = {};
            skybox = nullptr;
        }
    }
}
