#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "ECS/ECS.h"

class GameObject : public ECS::Entity<GameObject> {
protected:
    void OnDelete() override;

public:
    std::string Name = "GameObject";
    std::string Tag;

    GameObject() {}
    ~GameObject() {}

    json SerializeObj() override;
    void UnSerializeObj(const json &j) override;

};

#endif
