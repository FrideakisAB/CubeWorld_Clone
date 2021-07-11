#include "ECS/ISystem.h"

namespace ECS {
    ISystem::ISystem(SystemPriority priority, f32 updateInterval_ms) :
            priority(priority),
            updateInterval(updateInterval_ms),
            enabled(true) {}
}