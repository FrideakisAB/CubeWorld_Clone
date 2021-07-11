#ifndef COMPONENT_H
#define COMPONENT_H

#include "ECS/API.h"
#include "ECS/IComponent.h"
#include "ECS/util/FamilyTypeID.h"

namespace ECS {
    template<class T>
    class Component : public IComponent {
    public:
        static const ComponentTypeId STATIC_COMPONENT_TYPE_ID;

        ~Component() override = default;

        [[nodiscard]] inline ComponentTypeId GetStaticComponentTypeID() const
        {
            return STATIC_COMPONENT_TYPE_ID;
        }
    };

    template<class T>
    const ComponentTypeId Component<T>::STATIC_COMPONENT_TYPE_ID = util::Internal::FamilyTypeID<IComponent>::Get<T>();
}

#endif 