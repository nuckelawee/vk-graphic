#include "input/input_controller.hpp"

namespace input {

void Controller::KeyInput(int key, int scancode, int action, int modes) {
    keyboard_.KeyInput(key, scancode, action, modes);
}

void Controller::CursorPosition(int x, int y) {
    mouse_.CursorPosition(x, y);
}

void Controller::MouseButton(int button, int action, int mods) {
    mouse_.Button(button, action, mods);
}

void Controller::SwitchContext(void *pContext, ContextType type) {
    context_.Switch(pContext, type);
}

void Controller::Update() {
    context_.UpdateKeys(keyboard_.Keys());
    context_.UpdateMousePosition(mouse_.DeltaXPos(), mouse_.DeltaYPos());
    mouse_.CursorPosition(mouse_.XPosition(), mouse_.YPosition());
}

} // input
