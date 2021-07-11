#include "ECS/Event/IEventListener.h"
#include "ECS/Engine.h"

namespace ECS::Event {
    IEventListener::~IEventListener()
    {
        UnregisterAllEventCallbacks();
    }

    void IEventListener::UnregisterAllEventCallbacks()
    {
        for (auto *cb : this->m_RegisteredCallbacks)
            ECS_Engine->UnsubscribeEvent(cb);

        this->m_RegisteredCallbacks.clear();
    }
}