#ifndef ENTITY_H
#define ENTITY_H

#include "ECS/Engine.h"
#include "ECS/IEntity.h"
#include "ECS/util/FamilyTypeID.h"

namespace ECS {
    template<class E>
    class Entity : public IEntity {
    public:
        static const EntityTypeId STATIC_ENTITY_TYPE_ID;

        ~Entity() override = default;

        void operator delete[](void *) {}
        void operator delete(void *) {}

        [[nodiscard]] const EntityTypeId GetStaticEntityTypeID() const override { return STATIC_ENTITY_TYPE_ID; }
    };

    template<class E>
    const EntityTypeId Entity<E>::STATIC_ENTITY_TYPE_ID = util::Internal::FamilyTypeID<IEntity>::Get<E>();
}

#endif