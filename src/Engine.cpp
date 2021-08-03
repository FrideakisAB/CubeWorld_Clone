#include "Engine.h"

#include "ECS/Engine.h"
#include "ECS/SystemManager.h"
#include "Memory/MemoryManager.h"
#include "Assets/AssetsManager.h"
#include "Systems/RenderSystem.h"

Engine* GameEngine = nullptr;

Engine::Engine()
{
    assetsManager = new AssetsManager();
    Memory::InitializeMemoryManager();
    ECS::Initialize();

    auto &SM = *ECS::ECS_Engine->GetSystemManager();
    renderSystem = SM.AddSystem<RenderSystem>();
    SM.DisableSystem<RenderSystem>();
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
