#ifndef SCENEVIEWCOMMANDS_H
#define SCENEVIEWCOMMANDS_H

#include <string>
#include "ECS/ECS.h"
#include <functional>
#include "Editor/CacheSystem.h"
#include "Editor/Commands/ICommand.h"

class DeleteGO final : public ICommand {
private:
    ECS::EntityId &select;
    GameObject *gameObject, *parent = nullptr;
    u64 gameObjectId = 0, parentId = 0;
    CacheEntry cache;

public:
    DeleteGO(ECS::EntityId entityId, ECS::EntityId &select);
    ~DeleteGO() final;

    void Execute() final;
    void Undo() final;
};

class AddChild final : public ICommand {
private:
    GameObject *child;
    GameObject *parent;
    GameObject *prevParent = nullptr;
    u64 childId = 0, parentId = 0, prevParentId = 0;

public:
    AddChild(ECS::EntityId childId, ECS::EntityId parentId);

    void Execute() final;
    void Undo() final;
};

class RemoveParent final : public ICommand {
private:
    GameObject *child;
    GameObject *prevParent = nullptr;
    u64 childId = 0, prevParentId = 0;

public:
    explicit RemoveParent(ECS::EntityId childId);

    void Execute() final;
    void Undo() final;
};

class CustomCreate final : public ICommand {
private:
    ECS::EntityId &select;
    ECS::EntityId *resultId;
    GameObject *go = nullptr;
    u64 goId = 0;
    std::function<void(GameObject*)> func;

public:
    CustomCreate(ECS::EntityId *resultId, const std::function<void(GameObject*)> &func, ECS::EntityId &select);

    void Execute() final;
    void Undo() final;
};

#endif
