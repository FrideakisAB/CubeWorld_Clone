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
        util::Timer *engineTime;
        EntityManager *entityManager;
        ComponentManager *componentManager;
        SystemManager *systemManager;
        Event::EventHandler *eventHandler;
        ComponentFactory *componentFactory;

        ECSEngine(const ECSEngine &) = delete;
        ECSEngine &operator=(ECSEngine &) = delete;

        template<class E>
        inline void SubscribeEvent(Event::Internal::IEventDelegate *const eventDelegate)
        {
            eventHandler->AddEventCallback<E>(eventDelegate);
        }

        void UnsubscribeEvent(Event::Internal::IEventDelegate *eventDelegate);

    public:
        ECSEngine();
        ~ECSEngine();

        [[nodiscard]] inline EntityManager *GetEntityManager() const noexcept { return entityManager; }
        [[nodiscard]] inline ComponentManager *GetComponentManager() const noexcept { return componentManager; }
        [[nodiscard]] inline SystemManager *GetSystemManager() const noexcept { return systemManager; }
        [[nodiscard]] inline ComponentFactory *GetComponentFactory() const noexcept { return componentFactory; }
        [[nodiscard]] inline util::Timer *GetTimer() const noexcept { return engineTime; }

        template<class E, class... ARGS>
        void SendEvent(ARGS&&... eventArgs)
        {
            eventHandler->Send<E>(std::forward<ARGS>(eventArgs)...);
        }

        void Update();
        void RemoveDestroyedEntities();
    };
}

#endif 