#include "input/input_keyboard.hpp"

#include <cassert>

namespace input {

cmd::Instruction Keyboard::KeyInput(int key, int scancode
    , int action, int modes) const noexcept {

    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        assert(key != GLFW_KEY_UNKNOWN);
        return commands_[key];
    }
    return nullCmd_;
}

void Keyboard::SetCommand(const cmd::Instruction& command, int key) noexcept {
    commands_[key] = command;  
}

void Keyboard::SetCommand(cmd::Instruction&& command, int key) noexcept {
    commands_[key] = std::move(command);
}

} // input
