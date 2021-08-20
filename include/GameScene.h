#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <map>
#include <string>
#include <vector>
#include "ECS/ECS.h"
#include "GameObject.h"
#include "Assets/IAsset.h"
#include "boost/type_index.hpp"

class GameScene : public IAsset {
    friend GameObject;

private:
    std::map<ECS::EntityId, GameObject*> gameObjects;

    void RemoveObject(ECS::EntityId eid);
    void Validate(ECS::EntityId eid, GameObject *gameObject);

public:
    ~GameScene();

    GameObject *Create(const std::string &name, const std::string &tag="");
    void Delete(GameObject *go);
    void Delete(ECS::EntityId eid);

    [[nodiscard]] GameObject *Find(const std::string &name);
    [[nodiscard]] GameObject *FindByTag(const std::string &tag);

    [[nodiscard]] std::vector<GameObject*> FindAll(const std::string &name);
    [[nodiscard]] std::vector<GameObject*> FindByTagAll(const std::string &tag);

    [[nodiscard]] size_t GetTypeID() const noexcept override
    {
        return boost::typeindex::type_id<GameScene>().hash_code();
    }

    [[nodiscard]] IAsset *Clone() const override;

    [[nodiscard]] json SerializeObj() override;
    void UnSerializeObj(const json &j) override;
};

#endif
