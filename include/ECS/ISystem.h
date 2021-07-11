#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "ECS/API.h"

namespace ECS {
    using SystemTypeId = TypeID;
    using SystemPriority = u16;

    static const SystemTypeId INVALID_SYSTEMID = INVALID_TYPE_ID;

    static const SystemPriority LOWEST_SYSTEM_PRIORITY =    std::numeric_limits<SystemPriority>::min();
    static const SystemPriority VERY_LOW_SYSTEM_PRIORITY =  99;
    static const SystemPriority LOW_SYSTEM_PRIORITY =       100;
    static const SystemPriority NORMAL_SYSTEM_PRIORITY =    200;
    static const SystemPriority MEDIUM_SYSTEM_PRIORITY =    300;
    static const SystemPriority HIGH_SYSTEM_PRIORITY =      400;
    static const SystemPriority VERY_HIGH_SYSTEM_PRIORITY = 401;
    static const SystemPriority HIGHEST_SYSTEM_PRIORITY =   std::numeric_limits<SystemPriority>::max();

    class ISystem {
        friend class SystemManager;

    private:
        f32 timeSinceLastUpdate = 0;
        SystemPriority priority;
        f32 updateInterval;

        u8 enabled: 1;
        u8 needsUpdate: 1;
        u8 reserved: 6;

    protected:
        ISystem(SystemPriority priority = NORMAL_SYSTEM_PRIORITY, f32 updateInterval_ms = -1.0f);

    public:
        virtual ~ISystem() = default;

        [[nodiscard]] virtual inline const SystemTypeId GetStaticSystemTypeID() const = 0;
        [[nodiscard]] virtual inline const char *GetSystemTypeName() const = 0;

        virtual void PreUpdate() = 0;
        virtual void Update() = 0;
        virtual void PostUpdate() = 0;
    };
}

#endif