#pragma once

#include "cmd_keyboard.hpp"

class Window;

namespace cmd {

class Exit : public Keyboard {
    Window* window_;

public:
    explicit Exit(Window* window);

    virtual void Exicute() override;

    bool operator==(const Exit& other);

    virtual ~Exit() = default;
};

}
