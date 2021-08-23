#include "Editor/Commands/CommandList.h"

#include "imgui.h"

CommandList::CommandList()
{
    cmdBuffer.emplace_back(new EmptyCommand());
    position = cmdBuffer.begin();
}

CommandList::~CommandList()
{
    for (auto* cmd : cmdBuffer)
        delete cmd;
}

void CommandList::clearBefore()
{
    auto It = position; ++It;
    while (It != cmdBuffer.end())
    {
        delete *It;
        ++It;
    }
    It = position; ++It;

    cmdBuffer.erase(It, cmdBuffer.end());
}

void CommandList::Redo()
{
    if (position != cmdBuffer.end())
        ++position;
    else
        return;

    if (position != cmdBuffer.end())
        (*position)->Execute();
    else
        position = --cmdBuffer.end();
}

void CommandList::Undo()
{
    if (position != cmdBuffer.end() && position != cmdBuffer.begin())
    {
        (*position)->Undo();
        --position;
    }
}

void CommandList::removeOverflow()
{
    if (cmdBuffer.size() <= StoryLength)
        return;

    while (cmdBuffer.size() > StoryLength)
        cmdBuffer.erase(++cmdBuffer.begin());
}

void CommandList::InvalidateAll()
{
    auto It = cmdBuffer.begin(); ++It;
    while (It != cmdBuffer.end())
    {
        delete *It;
        ++It;
    }
    It = cmdBuffer.begin(); ++It;

    cmdBuffer.erase(It, cmdBuffer.end());

    entityMap.clear();
    recreateEntityMap.clear();
    lastId = 1;

    position = cmdBuffer.begin();
}

bool CommandList::IsRedoActive() const noexcept
{
    return position != --cmdBuffer.end();
}

bool CommandList::IsUndoActive() const noexcept
{
    return position != cmdBuffer.end() && position != cmdBuffer.begin();
}
