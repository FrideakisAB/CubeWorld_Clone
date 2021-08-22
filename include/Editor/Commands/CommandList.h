#ifndef COMMANDLIST_H
#define COMMANDLIST_H

#include <map>
#include <list>
#include "Editor/Commands/ICommand.h"

class GameObject;

class CommandList {
    using Buffer = std::list<ICommand*>;

private:
    Buffer cmdBuffer;
    Buffer::iterator position;
    std::map<u64, EntityValidator::Entry> entityMap;
    std::map<GameObject*, u64> recreateEntityMap;
    u64 lastId = 1;

    void clearBefore();
    void removeOverflow();

public:
    CommandList();
    ~CommandList();

    template<typename T, typename ...ARGS>
    void AddCommand(ARGS&&... args)
    {
        removeOverflow();
        T *obj = new T(std::forward<ARGS>(args)...);
        obj->validator = EntityValidator(&entityMap, &recreateEntityMap, &lastId);
        if(position == --cmdBuffer.end())
            cmdBuffer.emplace_back(obj);
        else
        {
            clearBefore();
            cmdBuffer.emplace_back(obj);
        }
    }

    void InvalidateAll();

    void Redo();
    void Undo();

    static constexpr size_t StoryLength = 10000;
};

#endif
