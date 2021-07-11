#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "ECS/API.h"
#include "ECS/Engine.h"
#include "ECS/ISystem.h"
#include "ECS/util/FamilyTypeID.h"
#include "Memory/GlobalMemoryUser.h"
#include "Memory/Allocator/LinearAllocator.h"

namespace ECS {
    using SystemWorkStateMask = std::vector<bool>;

    class SystemManager final : ::Memory::GlobalMemoryUser {
        friend ECSEngine;

        using SystemDependencyMatrix = std::vector<std::vector<bool>>;
        using SystemRegistry = std::unordered_map<u64, ISystem *>;
        using SystemAllocator = Memory::Allocator::LinearAllocator;
        using SystemWorkOrder = std::vector<ISystem *>;

    private:
        SystemAllocator *systemAllocator;
        SystemRegistry systems;
        SystemDependencyMatrix systemDependencyMatrix;
        SystemWorkOrder systemWorkOrder;

        SystemManager(const SystemManager &) = delete;
        SystemManager &operator=(SystemManager &) = delete;

        void Update();

    public:
        SystemManager();
        ~SystemManager() final;

        template<class T, class... ARGS>
        T *AddSystem(ARGS &&... systemArgs)
        {
            static const u64 STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = this->systems.find(STID);
            if ((this->systems.find(STID) != this->systems.end()) && (it->second != nullptr))
                return (T*)it->second;

            T *system = nullptr;
            void *pSystemMem = this->systemAllocator->allocate(sizeof(T), alignof(T));
            if (pSystemMem != nullptr)
            {
                ((T*)pSystemMem)->systemManagerInstance = this;
                system = new(pSystemMem)T(std::forward<ARGS>(systemArgs)...);
                this->systems[STID] = system;
            }
            else
            {
                logger->Error("Memory for System is out!");
                return system;
            }

            if (STID + 1 > this->systemDependencyMatrix.size())
            {
                this->systemDependencyMatrix.resize(STID + 1);
                for (auto &sys : this->systemDependencyMatrix)
                    sys.resize(STID + 1);
            }

            this->systemWorkOrder.push_back(system);

            return system;
        }

        template<class System, class Dependency>
        void AddSystemDependency(System target, Dependency dependency)
        {
            const u64 TARGET_ID = target->GetStaticSystemTypeID();
            const u64 DEPEND_ID = dependency->GetStaticSystemTypeID();

            this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;
        }

        template<class Target, class Dependency, class... Dependencies>
        void AddSystemDependency(Target target, Dependency dependency, Dependencies &&... dependencies)
        {
            const u64 TARGET_ID = target->GetStaticSystemTypeID();
            const u64 DEPEND_ID = dependency->GetStaticSystemTypeID();

            this->systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;

            this->AddSystemDependency(target, std::forward<Dependencies>(dependencies)...);
        }

        void UpdateSystemWorkOrder();

        template<class T>
        inline T *GetSystem() const
        {
            auto it = this->systems.find(T::STATIC_SYSTEM_TYPE_ID);

            return it != this->systems.end() ? (T *)it->second : nullptr;
        }

        template<class T>
        void EnableSystem()
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = this->systems.find(STID);
            if (it != this->systems.end())
                it->second->enabled = true;
            else
                logger->Warning("System not added: %s", typeid(T).name());
        }

        template<class T>
        void DisableSystem()
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = this->systems.find(STID);
            if (it != this->systems.end())
                it->second->enabled = false;
            else
                logger->Warning(std::string("System not added: " + std::string(typeid(T).name())));
        }

        template<class T>
        void SetSystemUpdateInterval(f32 interval_ms)
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = this->systems.find(STID);
            if (it != this->systems.end())
                it->second->updateInterval = interval_ms;
            else
                logger->Warning(std::string("System not added: " + std::string(typeid(T).name())));
        }

        template<class T>
        void SetSystemPriority(SystemPriority newPriority)
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = this->systems.find(STID);
            if (it != this->systems.end())
            {
                it->second->priority = newPriority;
                // re-build system work order
                this->UpdateSystemWorkOrder();
            }
            else
                logger->Warning(std::string("System not added: " + std::string(typeid(T).name())));
        }

        [[nodiscard]] SystemWorkStateMask GetSystemWorkState() const;
        void SetSystemWorkState(SystemWorkStateMask mask);

        template<class... ActiveSystems>
        SystemWorkStateMask GenerateActiveSystemWorkState(ActiveSystems &&... activeSystems)
        {
            SystemWorkStateMask mask(this->systemWorkOrder.size(), false);

            std::list<ISystem *> AS = {activeSystems...};
            for (auto s : AS)
                for (int i = 0; i < this->systemWorkOrder.size(); ++i)
                    if (this->systemWorkOrder[i]->GetStaticSystemTypeID() == s->GetStaticSystemTypeID())
                    {
                        mask[i] = true;
                        break;
                    }

            return mask;
        }
    };
}

#endif 