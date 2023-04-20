#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

#include "cmd/cmd_instruction.hpp"

namespace input {

class Keyboard {

    cmd::Instruction nullCmd_;
    std::array<cmd::Instruction, GLFW_KEY_LAST> commands_;

public:

    cmd::Instruction KeyInput(int key, int scancode, int action, int modes) const noexcept;

    void SetCommand(const cmd::Instruction& command, int key) noexcept;
    void SetCommand(cmd::Instruction&& command, int key) noexcept;

};

} // input
