#ifndef EVENTDELEGATE_H
#define EVENTDELEGATE_H

#include "Platform.h"

namespace ECS::Event {
    class IEvent;

    namespace Internal {
        using EventDelegateId = size_t;

        class IEventDelegate {
        public:
            virtual ~IEventDelegate() = default;
            virtual inline void invoke(const IEvent *const e) = 0;
            virtual inline EventDelegateId GetDelegateId() const = 0;
            virtual inline u64 GetStaticEventTypeId() const = 0;
            virtual bool operator==(const IEventDelegate *other) const = 0;
            virtual IEventDelegate *clone() = 0;
        };

        template<class Class, class EventType>
        class EventDelegate : public IEventDelegate {
            typedef void(Class::*Callback)(const EventType *const);

            Class *m_Receiver;
            Callback m_Callback;

        public:
            EventDelegate(Class *receiver, Callback &callbackFunction) :
                    m_Receiver(receiver),
                    m_Callback(callbackFunction) {}

            IEventDelegate *clone() override
            {
                return new EventDelegate(this->m_Receiver, this->m_Callback);
            }

            inline void invoke(const IEvent *const e) override
            {
                (m_Receiver->*m_Callback)(reinterpret_cast<const EventType *const>(e));
            }

            inline EventDelegateId GetDelegateId() const override
            {
                static const EventDelegateId DELEGATE_ID{typeid(Class).hash_code() ^ typeid(Callback).hash_code()};
                return DELEGATE_ID;
            }

            inline u64 GetStaticEventTypeId() const override
            {
                static const u64 SEID{EventType::STATIC_EVENT_TYPE_ID};
                return SEID;
            }

            bool operator==(const IEventDelegate *other) const override
            {
                if (other == nullptr)
                    return false;
                if (this->GetDelegateId() != other->GetDelegateId())
                    return false;

                const auto *delegate = static_cast<const EventDelegate *>(other);
                return ((this->m_Callback == delegate->m_Callback) && (this->m_Receiver == delegate->m_Receiver));
            }
        };
    }
}

#endif