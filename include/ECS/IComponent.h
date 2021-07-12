#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "ECS/IEntity.h"
#include "ECS/Serialization/ISerialize.h"

namespace ECS {
    using ComponentId = ObjectID;
    using ComponentTypeId = TypeID;

    static const ComponentId INVALID_COMPONENT_ID = INVALID_OBJECT_ID;

    class IComponent : public ISerialize {
        friend class ComponentManager;

    protected:
        ComponentId hashValue;
        ComponentId componentId;
        EntityId owner;
        u8 enabled : 1;
        u8 treeLock : 1;
        u8 reserve : 6;

        virtual void UpdateTree() {};

    public:
        IComponent();
        virtual ~IComponent() = default;

        inline const bool operator==(const IComponent &other) const { return hashValue == other.hashValue; }
        inline const bool operator!=(const IComponent &other) const { return hashValue != other.hashValue; }

        [[nodiscard]] inline const ComponentId GetComponentId() const { return this->componentId; }
        [[nodiscard]] inline const EntityId GetOwner() const { return this->owner; }

        inline void SetActive(bool active);
        [[nodiscard]] inline bool IsActive() const { return this->enabled; }

        virtual void OnEnable() {}
        virtual void OnDisable() {}
    };
}

#endif