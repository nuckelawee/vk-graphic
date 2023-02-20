#include "input/input_keyboard.hpp"
#include "input/input_context.hpp"

namespace input {

void Keyboard::KeyInput(Context& context, int key, int scancode, int action, int modes) {
    pKeys_[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
}

const bool* Keyboard::Keys() const {
    return pKeys_;
}


} // input
