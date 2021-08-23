#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>
#include <typeinfo>
#include <boost/type_index.hpp>
#include "Editor/Commands/EntityValidator.h"

class ICommand {
    friend class CommandList;

protected:
    std::string name;
    EntityValidator validator;

public:
    virtual ~ICommand() = default;

    [[nodiscard]] const std::string &GetName() const noexcept { return name; }
    virtual void Execute() = 0;
    virtual void Undo() = 0;
};

class EmptyCommand final : public ICommand {
public:
    EmptyCommand()
    {
        name = boost::typeindex::type_id<EmptyCommand>().pretty_name();
    }

    void Execute() final {}
    void Undo() final {}
};

#endif
