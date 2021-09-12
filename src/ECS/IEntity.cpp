#include "ECS/IEntity.h"

#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/Serialization/EntityFactory.h"
#include "ECS/Serialization/ComponentFactory.h"

namespace ECS {
    IEntity::IEntity() : localActive(true), globalActive(true), destroyed(false) {}

    json IEntity::SerializeComponents() const
    {
        json s;
        s["cmps"] = {""};

        const size_t NUM_COMPONENTS = this->componentManagerInstance->entityComponentMap[0].size();
        for (ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
        {
            const ECS::ComponentId componentId = this->componentManagerInstance->entityComponentMap[this->GetEntityID().index][componentTypeId];
            if (componentId == INVALID_COMPONENT_ID)
                continue;

            IComponent *component = this->componentManagerInstance->componentLut[componentId];
            if (component != nullptr)
            {
                json cmp = component->SerializeObj();
                s[std::string(cmp["cmpName"])] = cmp;
                s["cmps"].insert(s["cmps"].begin(), std::string(cmp["cmpName"]));
            }
        }

        return s;
    }

    void IEntity::UnSerializeComponents(const json &j)
    {
        for (auto &elm : j["cmps"])
        {
            if (elm != "")
            {
                IComponent *pCmp = ECS_Engine->GetComponentFactory()->Add(std::string(elm), this);
                pCmp->UnSerializeObj(j[std::string(elm)]);
            }
        }
    }

    void IEntity::SetActive(bool active)
    {
        if (this->localActive == active)
            return;

        if (!active)
            this->OnDisable();
        else
            this->OnEnable();

        this->localActive = active;

        if (globalActive == true || parentId == INVALID_ENTITY_ID)
            recSetActive();
    }

    void IEntity::onDelete()
    {
        destroyed = true;
        if (parentId != INVALID_ENTITY_ID)
            ECS_Engine->GetEntityManager()->GetEntity(parentId)->RemoveChild(entityId);

        auto It = childIds.begin();
        while (It != childIds.end())
        {
            ECS_Engine->GetEntityManager()->DestroyEntity(*It);
            It = childIds.begin();
        }
    }

    void IEntity::recSetActive()
    {
        for (auto entId : childIds)
        {
            auto *entity = ECS_Engine->GetEntityManager()->GetEntity(entId);
            entity->globalActive = IsActive();
            entity->recSetActive();
        }
    }

    void IEntity::SetParent(IEntity *entity)
    {
        if (entity != nullptr)
            entity->AddChild(this);
        else if (parentId != INVALID_ENTITY_ID)
            ECS_Engine->GetEntityManager()->GetEntity(parentId)->RemoveChild(entityId);
    }

    void IEntity::SetParent(EntityId entityId)
    {
        if (entityId != INVALID_ENTITY_ID)
        {
            auto *entity = ECS_Engine->GetEntityManager()->GetEntity(entityId);
            entity->AddChild(this);
        }
        else if (parentId != INVALID_ENTITY_ID)
            ECS_Engine->GetEntityManager()->GetEntity(parentId)->RemoveChild(this->entityId);
    }

    void IEntity::AddChild(IEntity *entity)
    {
        if (entity != nullptr && entity->parentId != entityId && entity->entityId != entityId && !entity->destroyed)
        {
            entity->SetParent(INVALID_ENTITY_ID);

            childIds.push_back(entity->entityId);
            entity->parentId = entityId;
            entity->globalActive = IsActive();
            entity->recSetActive();
            componentManagerInstance->UpdateTreeComponents(entity->entityId);
        }
    }

    void IEntity::AddChild(EntityId entityId)
    {
        if (entityId != INVALID_ENTITY_ID)
        {
            auto *entity = ECS_Engine->GetEntityManager()->GetEntity(entityId);
            AddChild(entity);
        }
    }

    void IEntity::RemoveChild(EntityId entityId)
    {
        if (entityId != INVALID_ENTITY_ID && entityId != this->entityId)
        {
            if (auto It = std::find(childIds.begin(), childIds.end(), entityId); It != childIds.end())
            {
                childIds.erase(It);
                auto *entity = ECS_Engine->GetEntityManager()->GetEntity(entityId);
                entity->parentId = INVALID_ENTITY_ID;
                entity->globalActive = true;
                entity->recSetActive();
                componentManagerInstance->UpdateTreeComponents(entity->entityId);
            }
        }
    }

    void IEntity::ReserveChildSpace(std::size_t count)
    {
        childIds.reserve(childIds.size() + count);
    }

    IEntity *IEntity::GetParent() const noexcept
    {
        return ECS_Engine->GetEntityManager()->GetEntity(parentId);
    }

    IEntity *IEntity::GetChild(std::size_t i) const
    {
        return ECS_Engine->GetEntityManager()->GetEntity(childIds[i]);
    }

    json IEntity::SerializeObj() const
    {
        json data;

        data["components"] = SerializeComponents();
        data["childCount"] = childIds.size();

        for(size_t i = 0; i < childIds.size(); ++i)
            data["child"][i] = ECS_Engine->GetEntityManager()->GetEntity(childIds[i])->SerializeObj();

        return data;
    }

    void IEntity::UnSerializeObj(const json &j)
    {
        auto &EF = *ECS_Engine->GetEntityFactory();
        auto &EM = *ECS_Engine->GetEntityManager();

        UnSerializeComponents(j["components"]);

        size_t childCount = j["childCount"];
        for (size_t i = 0; i < childCount; ++i)
        {
            IEntity *child = EM.GetEntity(EF.Create(j["child"][i]["base"]["ESID"]));
            AddChild(child);
            child->UnSerializeObj(j["child"][i]);
        }
    }
}
