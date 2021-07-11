#ifndef IENTITY_H
#define IENTITY_H

#include <vector>
#include "ECS/API.h"
#include "Utils/json.h"
#include "ECS/util/Handle.h"
#include "ECS/Serialization/ISerialize.h"

namespace ECS {
    using EntityTypeId = TypeID;
    using EntityId = util::Handle64;

    static const EntityId INVALID_ENTITY_ID = util::Handle64::INVALID_HANDLE;

    class IEntity : public ISerialize {
        friend class EntityManager;

    protected:
        ComponentManager *componentManagerInstance;
        EntityId entityId;
        EntityId parentId = INVALID_ENTITY_ID;
        std::vector<EntityId> childsId;
        u8 localActive : 1;
        u8 globalActive : 1;
        u8 reserved : 6;

        void recSetActive();
        void onDelete();

    public:
        IEntity();
        virtual ~IEntity() noexcept = default;

        json SerializeComponents();
        void UnSerializeComponents(const json &j);

        template<class T>
        T *GetComponent() const
        {
            return this->componentManagerInstance->template GetComponent<T>(this->entityId);
        }

        template<class T, class ...P>
        T *AddComponent(P&&... param)
        {
            return this->componentManagerInstance->template AddComponent<T>(this->entityId, std::forward<P>(param)...);
        }

        template<class T>
        void RemoveComponent()
        {
            this->componentManagerInstance->template RemoveComponent<T>(this->entityId);
        }

        inline bool operator==(const IEntity &rhs) const { return this->entityId == rhs.entityId; }
        inline bool operator!=(const IEntity &rhs) const { return this->entityId != rhs.entityId; }
        inline bool operator==(const IEntity *rhs) const { return this->entityId == rhs->entityId; }
        inline bool operator!=(const IEntity *rhs) const { return this->entityId != rhs->entityId; }

        [[nodiscard]] virtual const EntityTypeId GetStaticEntityTypeID() const = 0;

        [[nodiscard]] inline EntityId GetEntityID() const noexcept { return this->entityId; }
        [[nodiscard]] inline EntityId GetParentID() const noexcept { return this->parentId; }
        void SetParent(IEntity *entity);
        void SetParent(EntityId entityId);
        [[nodiscard]] inline EntityId GetChildID(std::size_t i) const { return childsId[i]; }
        [[nodiscard]] inline std::size_t GetChildCount() const noexcept { return childsId.size(); }
        void AddChild(IEntity *entity);
        void AddChild(EntityId entityId);
        void RemoveChild(EntityId entityId);

        void SetActive(bool active);
        [[nodiscard]] inline bool IsActive() const noexcept { return this->localActive && this->globalActive; }

        virtual void OnEnable() {}
        virtual void OnDisable() {}
    };
}

#endif 