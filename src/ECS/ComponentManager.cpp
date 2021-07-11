#include "ECS/ComponentManager.h"

namespace ECS {
    ComponentManager::ComponentManager()
    {
        this->entityComponentMap.resize(ENITY_LUT_GROW);
        for (auto i = 0; i < ENITY_LUT_GROW; ++i)
            this->entityComponentMap[i].resize(util::Internal::FamilyTypeID<IComponent>::GetCount(), INVALID_COMPONENT_ID);
    }

    ComponentManager::~ComponentManager()
    {
        for (auto cc : this->componentContainerRegistry)
        {
            delete cc.second;
            cc.second = nullptr;
        }
    }

    ComponentId ComponentManager::AcquireComponentId(IComponent *component)
    {
        int i = 0;
        for (; i < this->componentLut.size(); ++i)
        {
            if (this->componentLut[i] == nullptr)
            {
                this->componentLut[i] = component;
                return i;
            }
        }

        this->componentLut.resize(this->componentLut.size() + COMPONENT_LUT_GROW, nullptr);

        this->componentLut[i] = component;
        return i;
    }

    void ComponentManager::ReleaseComponentId(ComponentId id)
    {
        assert((id != INVALID_COMPONENT_ID && id < this->componentLut.size()) && "Invalid component id");
        this->componentLut[id] = nullptr;
    }

    void ComponentManager::MapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId)
    {
        const size_t NUM_COMPONENTS{util::Internal::FamilyTypeID<IComponent>::GetCount()};

        if ((this->entityComponentMap.size() - 1) < entityId.index)
        {
            size_t oldSize = this->entityComponentMap.size();
            size_t newSize = oldSize + ENITY_LUT_GROW;

            this->entityComponentMap.resize(newSize);

            for (auto i = oldSize; i < newSize; ++i)
                this->entityComponentMap[i].resize(NUM_COMPONENTS, INVALID_COMPONENT_ID);
        }

        this->entityComponentMap[entityId.index][componentTypeId] = componentId;
    }

    void ComponentManager::UnmapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId)
    {
        assert(this->entityComponentMap[entityId.index][componentTypeId] == componentId && "FATAL: Entity Component ID mapping corruption!");

        this->entityComponentMap[entityId.index][componentTypeId] = INVALID_COMPONENT_ID;
        this->ReleaseComponentId(componentId);
    }
} 