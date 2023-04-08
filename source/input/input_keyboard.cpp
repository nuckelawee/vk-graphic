#include "input/input_keyboard.hpp"
#include <iostream>

namespace input {

Keyboard::Keyboard() : keys_(1024) {}

void Keyboard::KeyInput(int key, int scancode, int action, int modes) noexcept {
    keys_[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
}

const std::vector<bool>& Keyboard::Keys() const noexcept {
    return keys_;
}


} // input
