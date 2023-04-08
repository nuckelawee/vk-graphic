#pragma once

#include "input_context.hpp"
#include "input_keyboard.hpp"
#include "input_mouse.hpp"

namespace input {

class Controller {

    Context context_;
    Keyboard keyboard_;
    Mouse mouse_;

    bool mouseFlag_ = false;

public:

    Controller() {}
    Controller(ContextType type, void* context);

    void KeyInput(int key, int scancode, int action, int modes) noexcept;
    void CursorPosition(int x, int y) noexcept;
    void MouseButton(int button, int action, int mods) noexcept;

    void Update() noexcept;

    void SwitchContext(ContextType type, void* context) noexcept;

};

} // input
