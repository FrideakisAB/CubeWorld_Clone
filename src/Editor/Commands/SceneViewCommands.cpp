#include "Editor/Commands/SceneViewCommands.h"

#include "Engine.h"
#include <filesystem>
#include "GameScene.h"
#include "GameObject.h"
#include "Editor/Editor.h"

namespace fs = std::filesystem;

DeleteGO::DeleteGO(ECS::EntityId entityId)
{
    name = boost::typeindex::type_id<DeleteGO>().pretty_name();

    gameObject = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(entityId));
    if (gameObject->GetEntityID() != ECS::INVALID_ENTITY_ID)
        parent = static_cast<GameObject*>(gameObject->GetParent());
}

DeleteGO::~DeleteGO()
{
    cache = GameEditor->CacheSystem.GetCache(cache.GetID());
    if (!cache.GetPath().empty())
        GameEditor->CacheSystem.RemoveCache(cache);
}

void DeleteGO::recMap(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        ids.push_back(validator.Map(child));
        recMap(child);
    }
}

void DeleteGO::recInv(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        validator.Invalidate(ids[pos++]);
        recInv(child);
    }
}

void DeleteGO::recVal(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        validator.Validate(ids[pos++], child);
        recVal(child);
    }
}

void DeleteGO::Execute()
{
    if (gameObjectId == 0)
    {
        gameObjectId = validator.Map(gameObject);
        recMap(gameObject);
    }

    if (parentId == 0 && parent != nullptr)
        parentId = validator.Map(parent);

    validator.Invalidate(gameObjectId);
    pos = 0;
    recInv(gameObject);

    cache = GameEditor->CacheSystem.CreateCache(8);
    std::ofstream file = std::ofstream(fs::current_path().string() + cache.GetPath());
    std::string jsonStr = gameObject->SerializeObj().dump(4);
    file.write(jsonStr.c_str(), jsonStr.size());
    file.close();

    if (gameObject->GetEntityID() == GameEditor->Selected)
        GameEditor->Selected = ECS::INVALID_ENTITY_ID;

    GameEngine->GetGameScene().Delete(gameObject);
}

void DeleteGO::Undo()
{
    cache = GameEditor->CacheSystem.GetCache(cache.GetID());
    if (!cache.GetPath().empty())
    {
        std::ifstream file = std::ifstream(fs::current_path().string() + cache.GetPath());
        GameObject* newGO = GameEngine->GetGameScene().Create("");
        newGO->UnSerializeObj(json_utils::TryParse(Utils::FileToString(std::move(file))));
        GameEditor->CacheSystem.RemoveCache(cache);

        if (parent != nullptr)
        {
            parent = validator.Get(parentId);
            newGO->SetParent(parent);
        }

        gameObject = newGO;

        validator.Validate(gameObjectId, gameObject);
        pos = 0;
        recVal(gameObject);
    }
}

AddChild::AddChild(ECS::EntityId childId, ECS::EntityId parentId)
{
    name = boost::typeindex::type_id<AddChild>().pretty_name();

    child = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(childId));
    parent = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(parentId));
    if (child->GetParentID() != ECS::INVALID_ENTITY_ID)
        prevParent = static_cast<GameObject*>(child->GetParent());
}

void AddChild::Execute()
{
    if (parentId == 0)
        parentId = validator.Map(parent);
    if (childId == 0)
        childId = validator.Map(child);
    if (prevParentId == 0 && prevParent != nullptr)
        prevParentId = validator.Map(prevParent);

    validator.Get(parentId)->AddChild(validator.Get(childId));
}

void AddChild::Undo()
{
    if (prevParent != nullptr)
        validator.Get(childId)->SetParent(validator.Get(prevParentId));
    else
        validator.Get(childId)->SetParent(nullptr);
}

RemoveParent::RemoveParent(ECS::EntityId childId)
{
    name = boost::typeindex::type_id<RemoveParent>().pretty_name();

    child = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(childId));
    if (child->GetParentID() != ECS::INVALID_ENTITY_ID)
        prevParent = static_cast<GameObject*>(child->GetParent());
}

void RemoveParent::Execute()
{
    if (childId == 0)
        childId = validator.Map(child);
    if (prevParentId == 0 && prevParent != nullptr)
        prevParentId = validator.Map(prevParent);

    validator.Get(childId)->SetParent(nullptr);
}

void RemoveParent::Undo()
{
    if (prevParent != nullptr)
        validator.Get(childId)->SetParent(validator.Get(prevParentId));
}

CustomCreate::CustomCreate(ECS::EntityId *resultId, const std::function<void(GameObject*)> &func)
    : resultId(resultId), func(func)
{
    name = boost::typeindex::type_id<CustomCreate>().pretty_name();
}

void CustomCreate::recMap(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        ids.push_back(validator.Map(child));
        recMap(child);
    }
}

void CustomCreate::recInv(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        validator.Invalidate(ids[pos++]);
        recInv(child);
    }
}

void CustomCreate::recVal(GameObject *go)
{
    for (size_t i = 0; i < go->GetChildCount(); ++i)
    {
        auto *child = static_cast<GameObject*>(go->GetChild(i));
        validator.Validate(ids[pos++], child);
        recVal(child);
    }
}

void CustomCreate::Execute()
{
    GameObject *secGO = GameEngine->GetGameScene().Create("load");
    if (goId == 0)
    {
        goId = validator.Map(secGO);
        recMap(secGO);
    }

    go = secGO;

    func(go);

    if (resultId != nullptr)
        *resultId = go->GetEntityID();

    validator.Validate(goId, secGO);
    pos = 0;
    recVal(secGO);
}

void CustomCreate::Undo()
{
    GameObject *secGO = validator.Get(goId);

    if (secGO->GetEntityID() == GameEditor->Selected)
        GameEditor->Selected = ECS::INVALID_ENTITY_ID;

    recInv(secGO);
    GameEngine->GetGameScene().Delete(secGO);
    validator.Invalidate(goId);
}
