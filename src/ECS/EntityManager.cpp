#include "ECS/EntityManager.h"

namespace ECS {
    EntityManager::EntityManager(ComponentManager *componentManagerInstance) :
            pendingDestroyedEntities(1024),
            numPendingDestroyedEntities(0),
            componentManagerInstance(componentManagerInstance) {}

    EntityManager::~EntityManager()
    {
        for (auto ec : this->entityRegistry)
        {
            delete ec.second;
            ec.second = nullptr;
        }
    }

    EntityId EntityManager::AcquireEntityId(IEntity *entity)
    {
        return this->entityHandleTable.AcquireHandle(entity);
    }

    void EntityManager::ReleaseEntityId(EntityId id)
    {
        this->entityHandleTable.ReleaseHandle(id);
    }

    void EntityManager::RemoveDestroyedEntities()
    {
        for (size_t i = 0; i < this->numPendingDestroyedEntities; ++i)
        {
            EntityId entityId = this->pendingDestroyedEntities[i];
            IEntity *entity = this->entityHandleTable[entityId];

            const EntityTypeId ETID = entity->GetStaticEntityTypeID();

            auto it = this->entityRegistry.find(ETID);
            if (it != this->entityRegistry.end())
            {
                this->componentManagerInstance->RemoveAllComponents(entityId);
                it->second->DestroyEntity(entity);
            }

            this->ReleaseEntityId(entityId);
        }

        this->numPendingDestroyedEntities = 0;
    }
} 