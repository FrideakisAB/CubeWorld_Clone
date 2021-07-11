#ifndef SYSTEM_H
#define SYSTEM_H

#include "ECS/API.h"
#include "ECS/ISystem.h"
#include "ECS/util/FamilyTypeID.h"

namespace ECS {
    template<class T>
    class System : public ISystem {
        friend class SystemManager;

    private:
        SystemManager *systemManagerInstance;

    protected:
        System(SystemPriority priority = NORMAL_SYSTEM_PRIORITY, f32 updateInterval_ms = -1.0f) :
                ISystem(priority, updateInterval_ms) {}

    public:
        static const SystemTypeId STATIC_SYSTEM_TYPE_ID;

        ~System() override = default;

        [[nodiscard]] inline const SystemTypeId GetStaticSystemTypeID() const override
        {
            return STATIC_SYSTEM_TYPE_ID;
        }

        [[nodiscard]] inline const char *GetSystemTypeName() const override
        {
            const char *SYSTEM_TYPE_NAME{typeid(T).name()};
            return SYSTEM_TYPE_NAME;
        }

        template<class... Dependencies>
        void AddDependencies(Dependencies &&... dependencies)
        {
            this->systemManagerInstance->AddSystemDependency(this, std::forward<Dependencies>(dependencies)...);
        }

        void PreUpdate() override {}
        void Update() override {}
        void PostUpdate() override {}
    };

    template<class T>
    const SystemTypeId System<T>::STATIC_SYSTEM_TYPE_ID = util::Internal::FamilyTypeID<ISystem>::Get<T>();
}

#endif