#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>
#include <typeinfo>
#include <boost/type_index.hpp>
#include "Editor/Commands/EntityValidator.h"

class ICommand {
    friend class CommandList;

protected:
    EntityValidator validator;

public:
    virtual ~ICommand() = default;

    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Finish() {}
};

class EmptyCommand final : public ICommand {
public:
    void Execute() final {}
    void Undo() final {}
};

#endif
