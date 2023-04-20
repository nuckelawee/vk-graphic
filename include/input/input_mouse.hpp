#pragma once

#include "cmd/cmd_instruction.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

namespace input {

class Mouse {

    float xPos_ = 0.0f;
    float yPos_ = 0.0f;

    std::array<cmd::Instruction, GLFW_MOUSE_BUTTON_LAST> buttonCmds_;
    cmd::Instruction cursorCmd_;

public:

    cmd::Instruction CursorPosition(float x, float y) noexcept;
    cmd::Instruction Button(int button, int action, int mods) noexcept;

    void SetButtonCommand(const cmd::Instruction& command, int key) noexcept;
    void SetButtonCommand(cmd::Instruction&& command, int key) noexcept;
    void SetCursorCommand(cmd::Instruction&& command) noexcept;

};

} // input
