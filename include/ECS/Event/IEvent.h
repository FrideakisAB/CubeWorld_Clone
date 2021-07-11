#ifndef IEVENT_H
#define IEVENT_H

#include "ECS/API.h"

namespace ECS::Event {
    using EventTypeId = TypeID;
    using EventTimestamp = TimeStamp;

    static const EventTypeId INVALID_EVENTTYPE = INVALID_TYPE_ID;

    class IEvent {
    private:
        EventTypeId typeId;
        EventTimestamp timeCreated;

    public:
        explicit IEvent(EventTypeId typeId);

        inline const EventTypeId GetEventTypeID() const { return this->typeId; }
        inline const EventTimestamp GetTimeCreated() const { return this->timeCreated; }
    };
}

#endif