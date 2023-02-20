#pragma once

#include "input_context.hpp"
#include "input_keyboard.hpp"
#include "input_mouse.hpp"

namespace input {

class Controller {

    Context context_;
    Keyboard keyboard_;
    Mouse mouse_;

public:

    void KeyInput(int key, int scancode, int action, int modes);
    void CursorPosition(int x, int y);
    void MouseButton(int button, int action, int mods);

    void Update();

    void SwitchContext(void *pContext, ContextType type);

    Controller() {}
    ~Controller() {}

};

} // input
