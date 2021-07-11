#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <list>
#include <algorithm>
#include "ECS/API.h"
#include "ECS/Event/IEventDispatcher.h"

namespace ECS::Event::Internal {
    template<class T>
    class EventDispatcher : public IEventDispatcher {
        using EventDelegateList = std::list<IEventDelegate *>;
        using PendingRemoveDelegates = std::list<IEventDelegate *>;

        PendingRemoveDelegates pendingRemoveDelegates;
        EventDelegateList eventCallbacks;
        bool locked;

    public:
        // never use!
        EventDispatcher() : locked(false) {}

        virtual ~EventDispatcher()
        {
            this->pendingRemoveDelegates.clear();
            this->eventCallbacks.clear();
        }

        inline void Dispatch(IEvent *event) override
        {
            this->locked = true;
            {
                if (this->pendingRemoveDelegates.empty() == false)
                {
                    for (auto EC : this->pendingRemoveDelegates)
                    {
                        auto result = std::find_if(this->eventCallbacks.begin(), this->eventCallbacks.end(),
                                                   [&](const IEventDelegate *other) {
                                                       return other->operator==(EC);
                                                   });

                        if (result != this->eventCallbacks.end())
                        {
                            auto *ptrMem = (IEventDelegate *)(*result);

                            this->eventCallbacks.erase(result);

                            delete ptrMem;
                            ptrMem = nullptr;
                        }
                    }
                    this->pendingRemoveDelegates.clear();
                }

                for (auto EC : this->eventCallbacks)
                {
                    assert(EC != nullptr && "Invalid event callback.");
                    EC->invoke(event);
                }
            }
            this->locked = false;
        }

        void AddEventCallback(IEventDelegate *const eventDelegate) override
        {
            auto result = std::find_if(this->pendingRemoveDelegates.begin(), this->pendingRemoveDelegates.end(),
                                       [&](const IEventDelegate *other) {
                                           return other->operator==(eventDelegate);
                                       });

            if (result != this->pendingRemoveDelegates.end())
            {
                this->pendingRemoveDelegates.erase(result);
                return;
            }

            this->eventCallbacks.push_back(eventDelegate);
        }

        void RemoveEventCallback(IEventDelegate *eventDelegate) override
        {
            if (this->locked == false)
            {
                auto result = std::find_if(this->eventCallbacks.begin(), this->eventCallbacks.end(),
                                           [&](const IEventDelegate *other) {
                                               return other->operator==(eventDelegate);
                                           });

                if (result != this->eventCallbacks.end())
                {
                    auto *ptrMem = (IEventDelegate *)(*result);

                    this->eventCallbacks.erase(result);

                    delete ptrMem;
                    ptrMem = nullptr;
                }
            }
            else
            {
                auto result = std::find_if(this->eventCallbacks.begin(), this->eventCallbacks.end(),
                                           [&](const IEventDelegate *other) {
                                               return other->operator==(eventDelegate);
                                           });

                assert(result != this->eventCallbacks.end() && "");
                this->pendingRemoveDelegates.push_back(*result);
            }
        }

        [[nodiscard]] inline size_t GetEventCallbackCount() const override { return this->eventCallbacks.size(); }
    };
}
#endif