#ifndef ECSAPI_H
#define ECSAPI_H

#define ENITY_LUT_GROW                  1024
#define ENITY_T_CHUNK_SIZE              512
#define COMPONENT_LUT_GROW              1024
#define COMPONENT_T_CHUNK_SIZE          512
#define ECS_EVENT_MEMORY_BUFFER_SIZE    4194304 // 4MB
#define ECS_SYSTEM_MEMORY_BUFFER_SIZE   8388608 // 8MB

#include "Log.h"
#include "Platform.h"

namespace ECS {
    namespace Event {
        class EventHandler;
    }

    class EntityManager;
    class SystemManager;
    class ComponentManager;

    class ECSEngine;
    extern ECSEngine *ECS_Engine;

    void Initialize();
    void Terminate();
}

#endif