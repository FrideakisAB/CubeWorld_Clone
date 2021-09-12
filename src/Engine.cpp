#include "Engine.h"

#include "ECS/Engine.h"
#include "Assets/Prefab.h"
#include "ECS/SystemManager.h"
#include "Memory/MemoryManager.h"
#include "Assets/AssetsManager.h"
#include "Systems/RenderSystem.h"
#include "Components/Components.h"
#include "Systems/ParticleWorker.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"

#include "GameScene.h"
#include "GameObject.h"
#include "Utils/Primitives.h"

Engine *GameEngine = nullptr;

Engine::Engine()
{
    std::random_device randomDevice;
    randomEngine = new std::mt19937(randomDevice());
    assetsManager = new AssetsManager();
    scene = new GameScene();
    Memory::InitializeMemoryManager();
    ECS::Initialize();

    auto &EM = *ECS::ECS_Engine->GetEntityManager();
    auto &SM = *ECS::ECS_Engine->GetSystemManager();
    renderSystem = SM.AddSystem<RenderSystem>();
    SM.AddSystem<ParticleWorker>();
    SM.DisableSystem<RenderSystem>();

    auto &assetsFactory = assetsManager->GetAssetsFactory();
    assetsFactory.Register<Mesh>();
    assetsFactory.Register<Prefab>();
    assetsFactory.Register<Texture>();
    assetsFactory.Register<Material>();

    auto &CF = (*ECS::ECS_Engine->GetComponentFactory());
    CF.Register<Camera>();
    CF.Register<LightSource>();
    CF.Register<MaterialComponent>();
    CF.Register<MeshComponent>();
    CF.Register<ParticleSystem>();
    CF.Register<Transform>();

    auto &EF = (*ECS::ECS_Engine->GetEntityFactory());
    EF.Register<GameObject>();

    assetsManager->AddAsset("Cube", Utils::CreateCube());
    assetsManager->AddAsset("Plane", Utils::CreatePlane());
    assetsManager->AddAsset("Sphere", Utils::CreateSphere(32));

    // Test scene
    auto *camera = scene->Create("Camera");
    Camera::Main = camera->AddComponent<Camera>();
    camera->AddComponent<Transform>();

    auto *sphere = scene->Create("Bloom sphere");
    auto pos = sphere->AddComponent<Transform>()->GetLocalPos();
    auto *mesh = sphere->AddComponent<MeshComponent>();
    auto *materialComponent = sphere->AddComponent<MaterialComponent>();
    pos.position.z = 18;
    sphere->GetComponent<Transform>()->SetLocalPos(pos);
    AssetsHandle materialHandle = std::make_shared<Material>();
    auto *material = static_cast<Material*>(materialHandle.get());
    material->Shader = "LightBloom";
    material->Uniforms["color_light"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 1.0f, 1.0f)};
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(assetsManager->GetAsset("Sphere"));

    auto *light = scene->Create("Spot light");
    pos = light->AddComponent<Transform>()->GetLocalPos();
    pos.position.z = 8;
    pos.position.y = 3;
    pos.rotate = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
    light->GetComponent<Transform>()->SetLocalPos(pos);
    auto *lightSource = light->AddComponent<LightSource>();
    lightSource->Type = LightType::Spot;
    lightSource->Radius = 50.0f;
    lightSource->CutterOff = 22.5f;

    auto *particles = scene->Create("Particles");
    pos = particles->AddComponent<Transform>()->GetLocalPos();
    particles->AddComponent<ParticleSystem>();
    materialComponent = particles->AddComponent<MaterialComponent>();
    pos.position.z = 18;
    pos.position.x = 5;
    particles->GetComponent<Transform>()->SetLocalPos(pos);
    materialHandle = std::make_shared<Material>();
    material = static_cast<Material*>(materialHandle.get());
    material->Shader = "ParticleCPU";
    materialComponent->SetMaterial(materialHandle);

    auto *plane = scene->Create("Plane");
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
    material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 0.0f, 0.0f)};
    material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
    materialComponent->SetMaterial(materialHandle);
    mesh->SetMesh(assetsManager->GetAsset("Plane"));
}

Engine::~Engine()
{
    delete scene;
    delete assetsManager;
    ECS::Terminate();
    Memory::TerminateMemoryManager();
    delete randomEngine;
}

void Engine::Update()
{
    ECS::ECS_Engine->Update();
}
