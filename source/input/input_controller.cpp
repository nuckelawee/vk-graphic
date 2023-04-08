#include "input/input_controller.hpp"

namespace input {

Controller::Controller(ContextType type, void* context) : context_(type, context) {}

void Controller::KeyInput(int key, int scancode, int action, int modes) noexcept {
    keyboard_.KeyInput(key, scancode, action, modes);
}

void Controller::CursorPosition(int x, int y) noexcept {
    mouseFlag_ = true; 
    mouse_.CursorPosition(x, y);
}

void Controller::MouseButton(int button, int action, int mods) noexcept {
    mouse_.Button(button, action, mods);
}

void Controller::SwitchContext(ContextType type, void* context) noexcept {
    context_.Switch(type, context);
}

void Controller::Update() noexcept {
    context_.UpdateKeys(keyboard_.Keys());

        context_.UpdateMousePosition(mouse_.DeltaXPos(), mouse_.DeltaYPos());
    if(mouseFlag_) {
        mouseFlag_ = false;
    }

    mouse_.CursorPosition(mouse_.XPosition(), mouse_.YPosition());
}

} // input
