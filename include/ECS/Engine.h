#ifndef ECSENGINE_H
#define ECSENGINE_H

#include "ECS/API.h"
#include "ECS/Event/EventHandler.h"
#include "ECS/Event/EventDelegate.h"

namespace ECS {
    class IEntity;
    class IComponent;
    class ISystem;

    namespace util {
        class Timer;
        namespace Internal {
            template<class T>
            class FamilyTypeID;
        }
    }

    namespace Event {
        class IEvent;
        class IEventListener;
        class EventHandler;
    }

    class EntityManager;
    class SystemManager;
    class ComponentManager;
    class ComponentFactory;

    class ECSEngine {
        friend IEntity;
        friend IComponent;
        friend ISystem;
        friend class Event::IEvent;
        friend class Event::IEventListener;
        friend class EntityManager;

    private:
        util::Timer *ECS_EngineTime;
        EntityManager *ECS_EntityManager;
        ComponentManager *ECS_ComponentManager;
        SystemManager *ECS_SystemManager;
        Event::EventHandler *ECS_EventHandler;
        ComponentFactory *ECS_ComponentFactory;

        ECSEngine(const ECSEngine &) = delete;
        ECSEngine &operator=(ECSEngine &) = delete;

        template<class E>
        inline void SubscribeEvent(Event::Internal::IEventDelegate *const eventDelegate)
        {
            ECS_EventHandler->AddEventCallback<E>(eventDelegate);
        }

        void UnsubscribeEvent(Event::Internal::IEventDelegate *eventDelegate);

    public:
        ECSEngine();
        ~ECSEngine();

        [[nodiscard]] inline EntityManager *GetEntityManager() const { return ECS_EntityManager; }
        [[nodiscard]] inline ComponentManager *GetComponentManager() const { return ECS_ComponentManager; }
        [[nodiscard]] inline SystemManager *GetSystemManager() const { return ECS_SystemManager; }
        [[nodiscard]] inline ComponentFactory *GetComponentFactory() const { return ECS_ComponentFactory; }
        [[nodiscard]] inline util::Timer *GetTimer() const { return ECS_EngineTime; }

        template<class E, class... ARGS>
        void SendEvent(ARGS&&... eventArgs)
        {
            ECS_EventHandler->Send<E>(std::forward<ARGS>(eventArgs)...);
        }

        void Update();
        void RemoveDestroyedEntities();
    };
}

#endif 