#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "Log.h"
#include "ECS/API.h"
#include "ECS/Engine.h"
#include "ECS/IComponent.h"
#include "ECS/util/FamilyTypeID.h"
#include "Memory/GlobalMemoryUser.h"
#include "Memory/Allocator/MemoryChunkAllocator.h"

class SceneEditor;

namespace ECS {
    class ComponentManager final : ::Memory::GlobalMemoryUser {
        friend SceneEditor;
        friend class IComponent;
        friend class IEntity;

        class IComponentContainer {
        public:
            virtual ~IComponentContainer() = default;

            [[nodiscard]] virtual const char *GetComponentContainerTypeName() const = 0;
            virtual void DestroyComponent(IComponent *object) = 0;
        };

        template<class T>
        class ComponentContainer : public ::Memory::Allocator::MemoryChunkAllocator<T, COMPONENT_T_CHUNK_SIZE>, public IComponentContainer {
            ComponentContainer(const ComponentContainer &) = delete;
            ComponentContainer &operator=(ComponentContainer &) = delete;

        public:
            ComponentContainer() : ::Memory::Allocator::MemoryChunkAllocator<T, COMPONENT_T_CHUNK_SIZE>("ComponentManager") {}

            ~ComponentContainer() override = default;

            [[nodiscard]] const char *GetComponentContainerTypeName() const override
            {
                const char *COMPONENT_TYPE_NAME{typeid(T).name()};
                return COMPONENT_TYPE_NAME;
            }

            void DestroyComponent(IComponent *object) override
            {
                object->~IComponent();
                this->DestroyObject(object);
            }
        };

    private:
        ComponentManager(const ComponentManager &) = delete;
        ComponentManager &operator=(ComponentManager &) = delete;

        using ComponentContainerRegistry = std::unordered_map<ComponentTypeId, IComponentContainer *>;
        ComponentContainerRegistry componentContainerRegistry;

        template<class T>
        inline ComponentContainer<T> *GetComponentContainer()
        {
            static const ComponentTypeId CID = T::STATIC_COMPONENT_TYPE_ID;

            auto it = this->componentContainerRegistry.find(CID);
            ComponentContainer<T> *cc = nullptr;

            if (it != this->componentContainerRegistry.end())
                cc = static_cast<ComponentContainer<T> *>(it->second);
            else
            {
                cc = new ComponentContainer<T>();
                this->componentContainerRegistry[CID] = cc;
            }

            assert(cc != nullptr && "Failed to create ComponentContainer<T>!");
            return cc;
        }

        using ComponentLookupTable = std::vector<IComponent *>;
        ComponentLookupTable componentLut;

        using EntityComponentMap = std::vector<std::vector<ComponentId>>;
        EntityComponentMap entityComponentMap;


        ComponentId AcquireComponentId(IComponent *component);
        void ReleaseComponentId(ComponentId id);

        void MapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId);
        void UnmapEntityComponent(EntityId entityId, ComponentId componentId, ComponentTypeId componentTypeId);

    public:
        template<class T>
        using TComponentIterator = typename ComponentContainer<T>::iterator;

        template<class T>
        class ComponentIterator {
        private:
            ComponentContainer<T> *container;

        public:
            explicit ComponentIterator(ComponentContainer<T> *container) : container(container) {}

            inline TComponentIterator<T> begin()
            {
                return container->begin();
            }

            inline TComponentIterator<T> end()
            {
                return container->end();
            }
        };

        ComponentManager();
        ~ComponentManager() final;

        template<class T, class ...ARGS>
        T *AddComponent(const EntityId entityId, ARGS &&... args)
        {
            constexpr std::hash<ComponentId> ENTITY_COMPONENT_ID_HASHER{std::hash<ComponentId>()};

            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;

            const ComponentId cmpId = this->entityComponentMap[entityId.index][CTID];
            if (cmpId != INVALID_COMPONENT_ID)
            {
                if (IComponent *oldCmp = this->componentLut[cmpId]; oldCmp != nullptr)
                    RemoveComponent<T>(entityId);
            }

            void *pObjectMemory = GetComponentContainer<T>()->CreateObject();

            ComponentId componentId = this->AcquireComponentId((T*)pObjectMemory);
            ((T*)pObjectMemory)->componentId = componentId;

            IComponent *component = new(pObjectMemory)T(std::forward<ARGS>(args)...);

            component->owner = entityId;
            component->hashValue = ENTITY_COMPONENT_ID_HASHER(entityId) ^ (ENTITY_COMPONENT_ID_HASHER(componentId) << 1);

            MapEntityComponent(entityId, componentId, CTID);

            return static_cast<T*>(component);
        }

        template<class T>
        void RemoveComponent(const EntityId entityId)
        {
            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
            const ComponentId componentId = this->entityComponentMap[entityId.index][CTID];

            IComponent *component = this->componentLut[componentId];

            assert(component != nullptr && "FATAL: Trying to remove a component which is not used by this entity!");

            GetComponentContainer<T>()->DestroyComponent(component);

            UnmapEntityComponent(entityId, componentId, CTID);
        }

        void RemoveComponent(const EntityId entityId, const ComponentId cmpId)
        {
            if (cmpId != ECS::INVALID_COMPONENT_ID)
            {
                ComponentTypeId componentTypeId = 0;
                auto &entityComponents = this->entityComponentMap[entityId.index];
                auto It = std::find_if(entityComponents.begin(), entityComponents.end(),
                                       [cmpId, &componentTypeId](const ComponentId &id) {
                                           if (id == cmpId)
                                               return true;

                                           ++componentTypeId;
                                           return false;
                                       });

                if (It != entityComponents.end())
                {
                    IComponent *component = this->componentLut[*It];
                    if (component != nullptr)
                    {
                        auto it = this->componentContainerRegistry.find(componentTypeId);
                        if (it != this->componentContainerRegistry.end())
                            it->second->DestroyComponent(component);

                        UnmapEntityComponent(entityId, *It, componentTypeId);
                    }
                }
            }
        }

        void RemoveAllComponents(const EntityId entityId)
        {
            const size_t NUM_COMPONENTS = this->entityComponentMap[0].size();

            for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
            {
                const ComponentId componentId = this->entityComponentMap[entityId.index][componentTypeId];
                if (componentId == INVALID_COMPONENT_ID)
                    continue;

                IComponent *component = this->componentLut[componentId];
                if (component != nullptr)
                {
                    auto it = this->componentContainerRegistry.find(componentTypeId);
                    if (it != this->componentContainerRegistry.end())
                        it->second->DestroyComponent(component);

                    UnmapEntityComponent(entityId, componentId, componentTypeId);
                }
            }
        }

        void UpdateTreeComponents(const EntityId entityId)
        {
            const size_t NUM_COMPONENTS = this->entityComponentMap[0].size();

            for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
            {
                const ComponentId componentId = this->entityComponentMap[entityId.index][componentTypeId];
                if (componentId == INVALID_COMPONENT_ID)
                    continue;

                IComponent *component = this->componentLut[componentId];
                if (component != nullptr && component->treeLock)
                    component->UpdateTree();
            }
        }

        template<class T>
        T *GetComponent(const EntityId entityId)
        {
            if (entityId == INVALID_ENTITY_ID)
                return nullptr;

            static const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
            const ComponentId componentId = this->entityComponentMap[entityId.index][CTID];

            if (componentId == INVALID_COMPONENT_ID)
                return nullptr;

            return static_cast<T*>(this->componentLut[componentId]);
        }

        template<class T>
        ComponentIterator<T> GetIterator()
        {
            return ComponentIterator<T>(GetComponentContainer<T>());
        }

        template<class T>
        inline TComponentIterator<T> begin()
        {
            return GetComponentContainer<T>()->begin();
        }

        template<class T>
        inline TComponentIterator<T> end()
        {
            return GetComponentContainer<T>()->end();
        }
    };
}

#endif