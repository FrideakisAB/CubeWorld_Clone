#ifndef SCENEEDITORCOMMANDS_H
#define SCENEEDITORCOMMANDS_H

#include "ECS/ECS.h"
#include "Editor/Commands/ICommand.h"

class TextEdit final : public ICommand {
private:
    std::string saveValue, newValue;
    GameObject *gameObject;
    u64 id = 0;
    bool isTag;

public:
    explicit TextEdit(GameObject *go, std::string value, bool isTag);

    void Execute() final;
    void Undo() final;
};

class AddComponent final : public ICommand {
private:
    GameObject *gameObject;
    u64 id = 0;
    std::string cmpName;
    ECS::ComponentId componentId = ECS::INVALID_COMPONENT_ID;

public:
    AddComponent(GameObject* go, std::string cmp);

    void Execute() final;
    void Undo() final;
};

class ActiveEdit final : public ICommand {
private:
    GameObject *gameObject;
    u64 id = 0;
    bool active;

public:
    explicit ActiveEdit(GameObject *go, bool active);

    void Execute() final;
    void Undo() final;
};

#endif
