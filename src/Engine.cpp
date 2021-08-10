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
    material->Shader = "LightAccumulation";
    Utils::ShaderParamValue uniform;
    uniform.value = glm::vec3(0.0f, 1.0f, 0.0f);
    uniform.valueType = Utils::ShaderValue::Vector3;
    material->Uniforms["color_diffuse"] = uniform;
    uniform.value = 0.5f;
    uniform.valueType = Utils::ShaderValue::Float;
    material->Uniforms["main_specular"] = uniform;
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(Utils::CreateSphere(32));

    auto *light = static_cast<GameObject*>(EM.GetEntity(EM.CreateEntity<GameObject>()));
    light->AddComponent<Transform>();
    auto *lightSource = light->AddComponent<LightSource>();
    lightSource->SetLightType(LightType::Point);
    lightSource->SetRadius(100);
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
