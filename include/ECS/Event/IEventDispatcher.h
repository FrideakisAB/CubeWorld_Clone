#ifndef IEVENTDISPATCHER_H
#define IEVENTDISPATCHER_H

#include <unordered_map>
#include "ECS/Event/EventDelegate.h"

namespace ECS::Event {
    class IEvent;

    namespace Internal {
        class IEventDispatcher {
        public:
            virtual ~IEventDispatcher() = default;

            virtual void Dispatch(IEvent *event) = 0;
            virtual void AddEventCallback(IEventDelegate *const eventDelegate) = 0;
            virtual void RemoveEventCallback(IEventDelegate *eventDelegate) = 0;
            virtual inline size_t GetEventCallbackCount() const = 0;
        };
    }
}

#endif