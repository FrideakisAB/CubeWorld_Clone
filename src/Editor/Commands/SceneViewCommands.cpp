#include "Editor/Commands/SceneViewCommands.h"

#include "Engine.h"
#include <filesystem>
#include "GameScene.h"
#include "GameObject.h"
#include "Editor/Editor.h"

namespace fs = std::filesystem;

DeleteGO::DeleteGO(ECS::EntityId entityId, ECS::EntityId &select)
    : select(select)
{
    name = typeid(DeleteGO).name();
    name = name.substr(name.find(' '));

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

void DeleteGO::Execute()
{
    if (gameObjectId == 0)
        gameObjectId = validator.Map(gameObject);

    if (parentId == 0 && parent != nullptr)
        parentId = validator.Map(parent);

    validator.Invalidate(gameObjectId);

    cache = GameEditor->CacheSystem.CreateCache(8);
    std::ofstream file = std::ofstream(fs::current_path().string() + cache.GetPath());
    std::string jsonStr = gameObject->SerializeObj().dump(4);
    file.write(jsonStr.c_str(), jsonStr.size());
    file.close();

    if (select == gameObject->GetEntityID())
        select = ECS::INVALID_ENTITY_ID;

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
    }
}

AddChild::AddChild(ECS::EntityId childId, ECS::EntityId parentId)
{
    name = typeid(AddChild).name();
    name = name.substr(name.find(' '));

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
    name = typeid(RemoveParent).name();
    name = name.substr(name.find(' '));

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

CustomCreate::CustomCreate(ECS::EntityId *resultId, const std::function<void(GameObject*)> &func, ECS::EntityId &select)
    : resultId(resultId), func(func), select(select)
{
    name = typeid(CustomCreate).name();
    name = name.substr(name.find(' '));
}

void CustomCreate::Execute()
{
    GameObject *secGO = GameEngine->GetGameScene().Create("load");
    if (goId == 0)
        goId = validator.Map(secGO);

    go = secGO;

    func(go);

    if (resultId != nullptr)
        *resultId = go->GetEntityID();

    validator.Validate(goId, secGO);
}

void CustomCreate::Undo()
{
    GameObject *secGO = validator.Get(goId);

    if (secGO->GetEntityID() == select)
        select = ECS::INVALID_ENTITY_ID;

    GameEngine->GetGameScene().Delete(secGO);
    validator.Invalidate(goId);
}
