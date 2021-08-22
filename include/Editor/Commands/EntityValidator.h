#ifndef ENTITYVALIDATOR_H
#define ENTITYVALIDATOR_H

#include <map>
#include "Platform.h"

class GameObject;

class EntityValidator {
public:
    struct Entry {
        GameObject *gameObject = nullptr;
    };

private:
    std::map<u64, Entry> *entityMap = nullptr;
    std::map<GameObject*, u64> *recreateEntityMap = nullptr;
    u64 *lastId = nullptr;

public:
    EntityValidator() = default;
    explicit EntityValidator(std::map<u64, Entry> *entityMap, std::map<GameObject*, u64> *recreateEntityMap, u64 *lastId)
    : entityMap(entityMap), recreateEntityMap(recreateEntityMap), lastId(lastId)
    {}

    u64 Map(GameObject *go);

    [[nodiscard]] GameObject *Get(u64 id) const;

    void Invalidate(u64 id);
    void Validate(u64 id, GameObject *newGo);
};

#endif
