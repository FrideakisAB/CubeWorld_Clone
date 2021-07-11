#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

#include <list>
#include "ECS/API.h"
#include "ECS/Event/EventDelegate.h"

namespace ECS::Event {
    class IEventListener {
        using RegisteredCallbacks = std::list<Internal::IEventDelegate *>;
        RegisteredCallbacks m_RegisteredCallbacks;

    public:
        IEventListener() = default;
        virtual ~IEventListener();

        template<class E, class C>
        inline void RegisterEventCallback(void(C::*Callback)(const E *const))
        {
            Internal::IEventDelegate *eventDelegate = new Internal::EventDelegate<C, E>(static_cast<C *>(this), Callback);

            m_RegisteredCallbacks.push_back(eventDelegate);
            ECS_Engine->template SubscribeEvent<E>(eventDelegate);
        }

        template<class E, class C>
        inline void UnregisterEventCallback(void(C::*Callback)(const E *const))
        {
            Internal::EventDelegate<C, E> delegate(static_cast<C *>(this), Callback);

            for (auto cb : this->m_RegisteredCallbacks)
            {
                if (cb->GetDelegateId() == delegate.GetDelegateId())
                {
                    this->m_RegisteredCallbacks.remove_if(
                            [&](const Internal::IEventDelegate *other) {
                                return other->operator==(cb);
                            }
                    );

                    ECS_Engine->UnsubscribeEvent(&delegate);
                    break;
                }
            }
        }

        void UnregisterAllEventCallbacks();
    };
}

#endif