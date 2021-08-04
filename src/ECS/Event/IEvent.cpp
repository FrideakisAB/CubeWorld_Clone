#include "ECS/Event/IEvent.h"
#include "ECS/Engine.h"
#include "ECS/util/Timer.h"

namespace ECS::Event {
    IEvent::IEvent(EventTypeId typeId) :
            typeId(typeId)
    {
        assert(ECS_Engine != nullptr && "ECS engine not initialized!");
        this->timeCreated = ECS_Engine->engineTime->GetTimeStamp();
    }
}
