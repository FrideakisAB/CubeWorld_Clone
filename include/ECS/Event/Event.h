#ifndef EVENT_H
#define EVENT_H

#include "ECS/Event/IEvent.h"
#include "ECS/util/FamilyTypeID.h"

namespace ECS::Event {
    template<class T>
    class Event : public IEvent {
    public:
        static const EventTypeId STATIC_EVENT_TYPE_ID;

        Event() : IEvent(STATIC_EVENT_TYPE_ID) {}
    };

    template<class T>
    const EventTypeId Event<T>::STATIC_EVENT_TYPE_ID{typeid(T).hash_code()};
}

#endif 