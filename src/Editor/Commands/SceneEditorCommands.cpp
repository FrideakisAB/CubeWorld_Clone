#include "Editor/Commands/SceneEditorCommands.h"

#include <utility>
#include "GameObject.h"

TextEdit::TextEdit(GameObject *go, std::string value, bool isTag)
    : gameObject(go), newValue(std::move(value)), isTag(isTag)
{
    name = boost::typeindex::type_id<TextEdit>().pretty_name();

    if(!isTag)
        saveValue = gameObject->Name;
    else
        saveValue = gameObject->Tag;
}

void TextEdit::Execute()
{
    if (id == 0)
        id = validator.Map(gameObject);

    if(!isTag)
        validator.Get(id)->Name = newValue;
    else
        validator.Get(id)->Tag = newValue;
}

void TextEdit::Undo()
{
    if(!isTag)
        validator.Get(id)->Name = saveValue;
    else
        validator.Get(id)->Tag = saveValue;
}

AddComponent::AddComponent(GameObject *go, std::string cmp)
    : gameObject(go), cmpName(std::move(cmp))
{
    name = boost::typeindex::type_id<AddComponent>().pretty_name();
}

void AddComponent::Execute()
{
    if (id == 0)
        id = validator.Map(gameObject);

    componentId = ECS::ECS_Engine->GetComponentFactory()->Add(cmpName, validator.Get(id))->GetComponentId();
}

void AddComponent::Undo()
{
    ECS::ECS_Engine->GetComponentManager()->RemoveComponent(validator.Get(id)->GetEntityID(), componentId);
}

ActiveEdit::ActiveEdit(GameObject *go, bool active)
: gameObject(go), active(active)
{
    name = boost::typeindex::type_id<AddComponent>().pretty_name();
}

void ActiveEdit::Execute()
{
    if (id == 0)
        id = validator.Map(gameObject);

    validator.Get(id)->SetActive(active);
}

void ActiveEdit::Undo()
{
    validator.Get(id)->SetActive(!active);
}
