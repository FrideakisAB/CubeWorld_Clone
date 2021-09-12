#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "ECS/API.h"
#include "ECS/Engine.h"
#include "ECS/IEntity.h"
#include "ECS/util/Handle.h"
#include "ECS/ComponentManager.h"
#include "Memory/Allocator/MemoryChunkAllocator.h"

#pragma warning(push)

// warning C4291: 'void *operator new(::size_t,void *) throw()': no matching operator delete found; memory will not be freed if initialization throws an exception
// note we are using custom memory allocator no need for delete
#pragma warning(disable: 4291)

namespace ECS {
    using EntityHandleTable = util::HandleTable<IEntity, EntityId>;

    class EntityManager {
        class IEntityContainer {
        public:
            virtual ~IEntityContainer() = default;

            [[nodiscard]] virtual const char *GetEntityContainerTypeName() const = 0;
            virtual void DestroyEntity(IEntity *object) = 0;
        };

        template<class T>
        class EntityContainer : public ::Memory::Allocator::MemoryChunkAllocator<T, ENITY_T_CHUNK_SIZE>, public IEntityContainer {
            EntityContainer(const EntityContainer &) = delete;
            EntityContainer &operator=(EntityContainer &) = delete;

        public:
            EntityContainer() : ::Memory::Allocator::MemoryChunkAllocator<T, ENITY_T_CHUNK_SIZE>("EntityManager") {}

            ~EntityContainer() override = default;

            [[nodiscard]] const char *GetEntityContainerTypeName() const override
            {
                const char *ENTITY_TYPE_NAME{typeid(T).name()};
                return ENTITY_TYPE_NAME;
            }

            void DestroyEntity(IEntity *object) override
            {
                object->~IEntity();
                this->DestroyObject(object);
            }
        };

    private:
        EntityManager(const EntityManager &) = delete;
        EntityManager &operator=(EntityManager &) = delete;

        using EntityRegistry = std::unordered_map<EntityTypeId, IEntityContainer *>;
        EntityRegistry entityRegistry;

        using PendingDestroyedEntities = std::vector<EntityId>;
        PendingDestroyedEntities pendingDestroyedEntities;
        size_t numPendingDestroyedEntities;

        ComponentManager *componentManagerInstance;

        EntityHandleTable entityHandleTable;

        template<class T>
        inline EntityContainer<T> *GetEntityContainer()
        {
            static const EntityTypeId EID = T::STATIC_ENTITY_TYPE_ID;

            auto it = this->entityRegistry.find(EID);
            EntityContainer<T> *ec = nullptr;

            if (it == this->entityRegistry.end())
            {
                ec = new EntityContainer<T>();
                this->entityRegistry[EID] = ec;
            } else
                ec = (EntityContainer<T> *) it->second;

            assert(ec != nullptr && "Failed to create EntityContainer<T>!");
            return ec;
        }

        EntityId AcquireEntityId(IEntity *entity);
        void ReleaseEntityId(EntityId id);

    public:
        explicit EntityManager(ComponentManager *componentManagerInstance);
        ~EntityManager();

        template<class T, class... ARGS>
        EntityId CreateEntity(ARGS &&... args)
        {
            void *pObjectMemory = GetEntityContainer<T>()->CreateObject();

            ECS::EntityId entityId = this->AcquireEntityId((T *)pObjectMemory);

            ((T*)pObjectMemory)->entityId = entityId;
            ((T*)pObjectMemory)->componentManagerInstance = this->componentManagerInstance;

            IEntity *entity = new(pObjectMemory)T(std::forward<ARGS>(args)...);

            return entityId;
        }

        void DestroyEntity(EntityId entityId)
        {
            if (entityId == INVALID_ENTITY_ID)
                return;

            IEntity *entity = this->entityHandleTable[entityId];

            if (!entity->destroyed)
            {
                if (this->numPendingDestroyedEntities < this->pendingDestroyedEntities.size())
                    this->pendingDestroyedEntities[this->numPendingDestroyedEntities++] = entityId;
                else
                {
                    this->pendingDestroyedEntities.push_back(entityId);
                    ++this->numPendingDestroyedEntities;
                }

                entity->onDelete();
                entity->OnDelete();
            }
        }

        inline IEntity *GetEntity(EntityId entityId)
        {
            return this->entityHandleTable[entityId];
        }

        [[nodiscard]] inline EntityId GetEntityId(EntityId::value_type index) const
        {
            return this->entityHandleTable[index];
        }

        void RemoveDestroyedEntities();
    };
}

#pragma warning(pop)

#endif
