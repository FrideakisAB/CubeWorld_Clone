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

    u64 timedId = 1;
    u64 currentTimedId = 0;
    f32 accumulateTime = 0.0f;
    bool isLastTimed = false;

    void clearBefore();
    void removeOverflow();
    void finishTimed();

public:
    CommandList();
    ~CommandList();

    template<typename T, typename ...ARGS>
    void AddCommand(ARGS&&... args)
    {
        removeOverflow();

        if (isLastTimed)
            finishTimed();

        isLastTimed = false;

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

    template<typename T, typename ...ARGS>
    u64 AddTimedCommand(ARGS&&... args)
    {
        removeOverflow();

        if (isLastTimed)
            finishTimed();

        isLastTimed = true;
        accumulateTime = 0.0f;

        T *obj = new T(std::forward<ARGS>(args)...);
        obj->validator = EntityValidator(&entityMap, &recreateEntityMap, &lastId);
        if(position == --cmdBuffer.end())
            cmdBuffer.emplace_back(obj);
        else
        {
            clearBefore();
            cmdBuffer.emplace_back(obj);
        }

        Redo();

        currentTimedId = timedId;

        return timedId++;
    }

    [[nodiscard]] bool IsTimedValid(u64 id) const noexcept { return currentTimedId != id && currentTimedId != 0; }

    void InvalidateAll();

    [[nodiscard]] bool IsRedoActive() const noexcept;
    [[nodiscard]] bool IsUndoActive() const noexcept;

    void Redo();
    void Undo();

    void Update();

    static constexpr size_t StoryLength = 10000;
    static constexpr f32 DelayToSave = 1.0f;
};

#endif
