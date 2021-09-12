#include "GameObject.h"

#include "Engine.h"
#include "GameScene.h"
#include <boost/type_index.hpp>

void GameObject::OnDelete()
{
    gameScene->RemoveObject(this->GetEntityID());
}

json GameObject::SerializeObj() const
{
    json data;

    data["base"] = IEntity::SerializeObj();
    data["base"]["ESID"] = boost::typeindex::type_id<GameObject>().pretty_name();

    data["name"] = Name;
    data["tag"] = Tag;

    return data;
}

void GameObject::UnSerializeObj(const json &j)
{
    IEntity::UnSerializeObj(j["base"]);

    Name = j["name"];
    Tag = j["tag"];

    GameEngine->GetGameScene().Validate(this->GetEntityID(), this);
}
