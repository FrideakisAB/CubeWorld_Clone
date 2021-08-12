#include "Engine.h"

#include "ECS/Engine.h"
#include "ECS/SystemManager.h"
#include "Memory/MemoryManager.h"
#include "Assets/AssetsManager.h"
#include "Systems/RenderSystem.h"

#include "GameObject.h"
#include "Utils/Primitives.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/LightSource.h"
#include "Components/MeshComponent.h"

Engine* GameEngine = nullptr;

Engine::Engine()
{
    assetsManager = new AssetsManager();
    Memory::InitializeMemoryManager();
    ECS::Initialize();

    auto &EM = *ECS::ECS_Engine->GetEntityManager();
    auto &SM = *ECS::ECS_Engine->GetSystemManager();
    renderSystem = SM.AddSystem<RenderSystem>();
    SM.DisableSystem<RenderSystem>();

    // Test scene
    auto *camera = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    Camera::Main = camera->AddComponent<Camera>();
    camera->AddComponent<Transform>();

    auto *sphere = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    auto pos = sphere->AddComponent<Transform>()->GetLocalPos();
    auto *mesh = sphere->AddComponent<MeshComponent>();
    auto *materialComponent = sphere->AddComponent<MaterialComponent>();
    pos.position.z = 18;
    sphere->GetComponent<Transform>()->SetLocalPos(pos);
    AssetsHandle materialHandle = std::make_shared<Material>();
    auto *material = static_cast<Material*>(materialHandle.get());
    material->Shader = "LightBloom";
    Utils::ShaderParamValue uniform;
    uniform.value = glm::vec3(1.0f, 1.0f, 1.0f);
    uniform.valueType = Utils::ShaderValue::Vector3;
    material->Uniforms["lightColor"] = uniform;
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(Utils::CreateSphere(32));

    auto *light = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    pos = light->AddComponent<Transform>()->GetLocalPos();
    pos.position.z = 8;
    pos.position.y = 3;
    //pos.rotate = glm::radians(glm::vec3(-45.0f, 0.0f, 0.0f));
    light->GetComponent<Transform>()->SetLocalPos(pos);
    auto *lightSource = light->AddComponent<LightSource>();
    lightSource->SetLightType(LightType::Point);
    lightSource->SetRadius(50.0f);

    auto *light2 = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    pos = light2->AddComponent<Transform>()->GetLocalPos();
    pos.position.z = 8;
    pos.position.y = -3;
    pos.position.x = 5;
    //pos.rotate = glm::radians(glm::vec3(-45.0f, 0.0f, 0.0f));
    light2->GetComponent<Transform>()->SetLocalPos(pos);
    lightSource = light2->AddComponent<LightSource>();
    lightSource->SetLightType(LightType::Point);
    lightSource->SetRadius(50.0f);

    auto *cube = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    pos = cube->AddComponent<Transform>()->GetLocalPos();
    mesh = cube->AddComponent<MeshComponent>();
    materialComponent = cube->AddComponent<MaterialComponent>();
    pos.position.z = 18;
    pos.position.x = 5;
    cube->GetComponent<Transform>()->SetLocalPos(pos);
    materialHandle = std::make_shared<Material>();
    material = static_cast<Material*>(materialHandle.get());
    material->Shader = "LightAccumulation";
    Utils::ShaderParamValue uniformNow;
    uniformNow.value = glm::vec3(0.0f, 1.0f, 0.0f);
    uniformNow.valueType = Utils::ShaderValue::Vector3;
    material->Uniforms["color_diffuse"] = uniformNow;
    uniformNow.value = 0.5f;
    uniformNow.valueType = Utils::ShaderValue::Float;
    material->Uniforms["main_specular"] = uniformNow;
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(Utils::CreateCube());

    auto *plane = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    pos = plane->AddComponent<Transform>()->GetLocalPos();
    mesh = plane->AddComponent<MeshComponent>();
    materialComponent = plane->AddComponent<MaterialComponent>();
    pos.position.x = -10;
    pos.position.y = -8;
    pos.position.z = 24;
    pos.scale = glm::vec3(10.0f);
    pos.rotate = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
    plane->GetComponent<Transform>()->SetLocalPos(pos);
    materialHandle = std::make_shared<Material>();
    material = static_cast<Material*>(materialHandle.get());
    material->Shader = "LightAccumulation";
    Utils::ShaderParamValue uniformPlane;
    uniformPlane.value = glm::vec3(1.0f, 0.0f, 0.0f);
    uniformPlane.valueType = Utils::ShaderValue::Vector3;
    material->Uniforms["color_diffuse"] = uniformPlane;
    uniformPlane.value = 0.1f;
    uniformPlane.valueType = Utils::ShaderValue::Float;
    material->Uniforms["main_specular"] = uniformPlane;
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(Utils::CreatePlane());
}

Engine::~Engine()
{
    ECS::Terminate();
    Memory::TerminateMemoryManager();
    delete assetsManager;
}

void Engine::Update()
{
    ECS::ECS_Engine->Update();
}
