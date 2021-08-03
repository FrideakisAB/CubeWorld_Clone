
#include <Engine.h>

#include "ECS/Engine.h"

#include "ECS/util/Timer.h"
#include "ECS/SystemManager.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/util/FamilyTypeID.h"
#include "ECS/Serialization/ComponentFactory.h"

namespace ECS {
    ECSEngine::ECSEngine()
    {
        ECS_EngineTime = new util::Timer();
        ECS_EventHandler = new Event::EventHandler();
        ECS_SystemManager = new SystemManager();
        ECS_ComponentManager = new ComponentManager();
        ECS_EntityManager = new EntityManager(this->ECS_ComponentManager);
        ECS_ComponentFactory = new ComponentFactory();
    }

    ECSEngine::~ECSEngine()
    {
        delete ECS_EntityManager;
        ECS_EntityManager = nullptr;

        delete ECS_EngineTime;
        ECS_EngineTime = nullptr;

        delete ECS_ComponentManager;
        ECS_ComponentManager = nullptr;

        delete ECS_SystemManager;
        ECS_SystemManager = nullptr;

        delete ECS_EventHandler;
        ECS_EventHandler = nullptr;

        delete ECS_ComponentFactory;
        ECS_ComponentFactory = nullptr;
    }

    void ECSEngine::Update()
    {
        ECS_EntityManager->RemoveDestroyedEntities();

        ECS_EngineTime->Tick();
        ECS_SystemManager->Update();
        ECS_EventHandler->DispatchEvents();
    }

    void ECSEngine::RemoveDestroyedEntities()
    {
        ECS_EntityManager->RemoveDestroyedEntities();
    }

    void ECSEngine::UnsubscribeEvent(Event::Internal::IEventDelegate *eventDelegate)
    {
        ECS_EventHandler->RemoveEventCallback(eventDelegate);
    }
}
