#include "input/input_controller.hpp"

namespace input {

void Controller::KeyInput(int key, int scancode, int action, int modes) {
    keyboard_.KeyInput(context_, key, scancode, action, modes);
}

void Controller::CursorPosition(int x, int y) {
    mouse_.CursorPosition(context_, x, y);
}

void Controller::MouseButton(int button, int action, int mods) {
    mouse_.Button(context_, button, action, mods);
}

void Controller::SwitchContext(void *pContext, ContextType type) {
    context_.Switch(pContext, type);
}

void Controller::Update() {
    context_.UpdateKeys(keyboard_.Keys());
}

} // input
