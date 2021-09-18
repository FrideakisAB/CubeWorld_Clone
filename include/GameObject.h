#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "ECS/ECS.h"

class GameScene;

class GameObject : public ECS::Entity<GameObject> {
    friend GameScene;
    friend class Prefab;

private:
    GameScene *gameScene = nullptr;
    bool isPrefab = false;

protected:
    void OnDelete() override;

public:
    std::string Name = "GameObject";
    std::string Tag;

    GameObject() {}
    ~GameObject() {}

    [[nodiscard]] bool IsPrefab() const noexcept { return isPrefab; }
    void UnPrefab() noexcept { isPrefab = false; }

    [[nodiscard]] json SerializeObj() const override;
    void UnSerializeObj(const json &j) override;
};

#endif
