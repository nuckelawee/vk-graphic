#include "input/input_keyboard.hpp"

namespace input {

void Keyboard::KeyInput(int key, int scancode, int action, int modes) {
    pKeys_[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
}

const bool* Keyboard::Keys() const {
    return pKeys_;
}


} // input
