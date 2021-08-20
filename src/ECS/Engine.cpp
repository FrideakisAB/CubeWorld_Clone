#include <Engine.h>

#include "ECS/Engine.h"

#include "ECS/util/Timer.h"
#include "ECS/SystemManager.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/util/FamilyTypeID.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"

namespace ECS {
    ECSEngine::ECSEngine()
    {
        engineTime = new util::Timer();
        eventHandler = new Event::EventHandler();
        systemManager = new SystemManager();
        componentManager = new ComponentManager();
        entityManager = new EntityManager(this->componentManager);
        componentFactory = new ComponentFactory();
        entityFactory = new EntityFactory();
    }

    ECSEngine::~ECSEngine()
    {
        delete entityManager;
        entityManager = nullptr;

        delete engineTime;
        engineTime = nullptr;

        delete componentManager;
        componentManager = nullptr;

        delete systemManager;
        systemManager = nullptr;

        delete eventHandler;
        eventHandler = nullptr;

        delete componentFactory;
        componentFactory = nullptr;

        delete entityFactory;
        entityFactory = nullptr;
    }

    void ECSEngine::Update()
    {
        entityManager->RemoveDestroyedEntities();

        engineTime->Tick();
        systemManager->Update();
        eventHandler->DispatchEvents();
    }

    void ECSEngine::RemoveDestroyedEntities()
    {
        entityManager->RemoveDestroyedEntities();
    }

    void ECSEngine::UnsubscribeEvent(Event::Internal::IEventDelegate *eventDelegate)
    {
        eventHandler->RemoveEventCallback(eventDelegate);
    }
}
