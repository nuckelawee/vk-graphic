#pragma once

#include "cmd_command.hpp"

namespace cmd {

class Mouse : public Command {
protected:
    int sensity_ = 500;

    float x_ = 0.0f;
    float y_ = 0.0f;

    float deltaX_ = 0.0f;
    float deltaY_ = 0.0f;

protected:
    explicit Mouse(CmdType type) : Command(type) {}

public:
    virtual void Extend(void* data) override final;
    virtual void Exicture() {}

    virtual ~Mouse() = default;
};

}//cmd
