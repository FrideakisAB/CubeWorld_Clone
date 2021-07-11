#include "ECS/Event/EventHandler.h"

namespace ECS::Event {
    EventHandler::EventHandler()
    {
        this->eventMemoryAllocator = new EventMemoryAllocator(ECS_EVENT_MEMORY_BUFFER_SIZE, Allocate(ECS_EVENT_MEMORY_BUFFER_SIZE, "EventHandler"));

        this->eventStorage.reserve(1024);
    }

    EventHandler::~EventHandler()
    {
        for (auto &it : this->eventDispatcherMap)
        {
            delete it.second;
            it.second = nullptr;
        }

        this->eventDispatcherMap.clear();

        Free((void *)this->eventMemoryAllocator->GetMemoryAddress0());

        delete this->eventMemoryAllocator;
        this->eventMemoryAllocator = nullptr;
    }
}