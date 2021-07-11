#include "ECS/util/FamilyTypeID.h"

namespace ECS {
    class IEntity;
    class IComponent;
    class ISystem;

    namespace util::Internal {
        template
        class FamilyTypeID<IEntity>;

        template
        class FamilyTypeID<IComponent>;

        template
        class FamilyTypeID<ISystem>;
    }
}