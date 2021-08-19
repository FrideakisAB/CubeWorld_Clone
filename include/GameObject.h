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

};

#endif
