#pragma once

#include "input_keyboard.hpp"
#include "input_mouse.hpp"
#include "cmd/cmd_instruction.hpp"

class BaseCamera;
class Window;

namespace input {

class Controller {

    Mouse mouse_;
    Keyboard keyboard_;

    cmd::Instruction keyCmd_;
    cmd::Instruction cursorCmd_;
    cmd::Instruction mouseCmd_;

public:

    Controller(BaseCamera& camera, Window& window) noexcept;

    void UpdateAll() noexcept;

    void KeyInput(int key, int scancode, int action, int modes) noexcept;
    void CursorPosition(int x, int y) noexcept;
    void MouseButton(int button, int action, int mods) noexcept;

private:
    void SetDefaultLayout(BaseCamera& camera, Window& window);

    void SetCallbacks(GLFWwindow* window) noexcept;

};

} // input
