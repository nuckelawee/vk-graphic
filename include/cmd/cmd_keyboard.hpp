#pragma once

#include "cmd_command.hpp"

namespace cmd {

class Keyboard : public Command {

protected:
    Keyboard(CmdType type) : Command(type) {}

public:

    virtual void Extend(void* data) override final {}

    virtual ~Keyboard() = default;
};


}// cmd
