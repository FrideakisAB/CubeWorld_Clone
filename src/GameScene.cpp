#include "GameScene.h"

GameScene::~GameScene()
{
    auto It = gameObjects.begin();
    while(It != gameObjects.end())
    {
        Delete(It->second);
        It = gameObjects.begin();
    }
}

GameObject *GameScene::Create(const std::string &name, const std::string &tag)
{
    auto *EM = ECS::ECS_Engine->GetEntityManager();

    auto id = EM->CreateEntity<GameObject>();
    auto *go = (GameObject*)EM->GetEntity(id);

    go->Name = name;
    go->Tag = tag;

    gameObjects.emplace(id, go);

    return go;
}

void GameScene::Delete(GameObject *go)
{
    if (go != nullptr)
        Delete(go->GetEntityID());
}

void GameScene::Delete(ECS::EntityId eid)
{
    ECS::ECS_Engine->GetEntityManager()->DestroyEntity(eid);
}

GameObject *GameScene::Find(const std::string &name)
{
    auto pos = std::find_if(gameObjects.begin(), gameObjects.end(), [&](std::pair<ECS::EntityId, GameObject*> pair){
        return pair.second->Name == name;
    });

    return pos != gameObjects.end()? pos->second : nullptr;
}

GameObject *GameScene::FindByTag(const std::string &tag)
{
    auto pos = std::find_if(gameObjects.begin(), gameObjects.end(), [&](std::pair<ECS::EntityId, GameObject*> pair){
        return pair.second->Tag == tag;
    });

    return pos != gameObjects.end()? pos->second : nullptr;
}

std::vector<GameObject*> GameScene::FindAll(const std::string &name)
{
    std::vector<GameObject*> findGOs;

    for(auto &&[eid, gameObject] : gameObjects)
    {
        if(gameObject->Name == name)
            findGOs.push_back(gameObject);
    }

    return findGOs;
}

std::vector<GameObject*> GameScene::FindByTagAll(const std::string &tag)
{
    std::vector<GameObject*> findGOs;

    for(auto &&[eid, gameObject] : gameObjects)
    {
        if(gameObject->Tag == tag)
            findGOs.push_back(gameObject);
    }

    return findGOs;
}

IAsset *GameScene::Clone() const
{
    return IAsset::Clone();
}

void GameScene::RemoveObject(ECS::EntityId eid)
{
    gameObjects.erase(gameObjects.find(eid));
}

json GameScene::SerializeObj()
{
    return {};
}

void GameScene::UnSerializeObj(const json &j)
{

}

void GameScene::SerializeBin(std::ofstream &file)
{

}

void GameScene::UnSerializeBin(std::ifstream &file)
{

}
