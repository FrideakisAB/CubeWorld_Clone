#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "ECS/ECS.h"

class GameScene;

class GameObject : public ECS::Entity<GameObject> {
    friend GameScene;

private:
    GameScene *gameScene = nullptr;

protected:
    void OnDelete() override;

public:
    std::string Name = "GameObject";
    std::string Tag;

    GameObject() {}
    ~GameObject() {}

    [[nodiscard]] json SerializeObj() const override;
    void UnSerializeObj(const json &j) override;
};

#endif
