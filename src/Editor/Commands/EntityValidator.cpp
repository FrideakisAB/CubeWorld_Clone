#include "Editor/Commands/EntityValidator.h"

u64 EntityValidator::Map(GameObject *go)
{
    if (recreateEntityMap->find(go) != recreateEntityMap->end())
        return (*recreateEntityMap)[go];
    else
    {
        u64 id = (*lastId);
        ++(*lastId);
        (*entityMap)[id].gameObject = go;
        (*recreateEntityMap)[go] = id;

        return id;
    }
}

GameObject *EntityValidator::Get(u64 id) const
{
    if (id < *lastId)
        return (*entityMap)[id].gameObject;
    else
        return nullptr;
}

void EntityValidator::Invalidate(u64 id)
{
    if (id < *lastId)
    {
        (*recreateEntityMap).erase((*entityMap)[id].gameObject);
        (*entityMap)[id].gameObject = nullptr;
    }
}

void EntityValidator::Validate(u64 id, GameObject *newGo)
{
    if (id < *lastId)
    {
        (*entityMap)[id].gameObject = newGo;
        (*recreateEntityMap)[newGo] = id;
    }
}
