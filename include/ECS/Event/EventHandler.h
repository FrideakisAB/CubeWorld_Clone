#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <vector>
#include "ECS/API.h"
#include "ECS/Event/IEvent.h"
#include "Memory/GlobalMemoryUser.h"
#include "ECS/Event/EventDispatcher.h"
#include "Memory/Allocator/LinearAllocator.h"

namespace ECS {
    class ECSEngine;
    namespace Event {
        class EventHandler : ::Memory::GlobalMemoryUser {
            friend ECSEngine;

            using EventDispatcherMap = std::unordered_map<EventTypeId, Internal::IEventDispatcher *>;
            using EventStorage = std::vector<IEvent *>;
            using EventMemoryAllocator = Memory::Allocator::LinearAllocator;

        private:
            EventHandler(const EventHandler &);
            EventHandler &operator=(EventHandler &);

            EventDispatcherMap eventDispatcherMap;
            EventMemoryAllocator *eventMemoryAllocator;
            EventStorage eventStorage;

            template<class E>
            inline void AddEventCallback(Internal::IEventDelegate *const eventDelegate)
            {
                EventTypeId ETID = E::STATIC_EVENT_TYPE_ID;

                EventDispatcherMap::const_iterator iter = this->eventDispatcherMap.find(ETID);
                if (iter == this->eventDispatcherMap.end())
                {
                    std::pair<EventTypeId, Internal::IEventDispatcher *> kvp(ETID, new Internal::EventDispatcher<E>());

                    kvp.second->AddEventCallback(eventDelegate);

                    this->eventDispatcherMap.insert(kvp);
                }
                else
                {
                    this->eventDispatcherMap[ETID]->AddEventCallback(eventDelegate);
                }
            }

            inline void RemoveEventCallback(Internal::IEventDelegate *eventDelegate)
            {
                auto typeId = eventDelegate->GetStaticEventTypeId();
                EventDispatcherMap::const_iterator iter = this->eventDispatcherMap.find(typeId);
                if (iter != this->eventDispatcherMap.end())
                    this->eventDispatcherMap[typeId]->RemoveEventCallback(eventDelegate);
            }

        public:
            EventHandler();
            ~EventHandler();

            inline void ClearEventBuffer()
            {
                this->eventMemoryAllocator->clear();
                this->eventStorage.clear();
            }

            inline void ClearEventDispatcher()
            {
                this->eventDispatcherMap.clear();
            }

            template<class E, class... ARGS>
            void Send(ARGS &&... eventArgs)
            {
                // check if type of object is trivially copyable
                static_assert(std::is_trivially_copyable<E>::value, "Event is not trivially copyable.");

                // allocate memory to store event data
                void *pMem = this->eventMemoryAllocator->allocate(sizeof(E), alignof(E));

                // add new event to buffer and event storage
                if (pMem != nullptr)
                    this->eventStorage.push_back(new(pMem)E(std::forward<ARGS>(eventArgs)...));
                else
                    logger->Error("Memory for Event is out! Event no send");
            }

            void DispatchEvents()
            {
                size_t lastIndex = this->eventStorage.size();
                size_t thisIndex = 0;

                while (thisIndex < lastIndex)
                {
                    auto event = this->eventStorage[thisIndex++];
                    if (event == nullptr)
                    {
                        logger->Error("Event corrupted");
                        continue;
                    }

                    auto it = this->eventDispatcherMap.find(event->GetEventTypeID());
                    if (it == this->eventDispatcherMap.end())
                        continue;

                    it->second->Dispatch(event);
                    lastIndex = this->eventStorage.size();
                }

                ClearEventBuffer();
            }
        };
    }
}

#endif