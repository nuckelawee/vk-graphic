#include "input/input_mouse.hpp"

namespace input {

cmd::Instruction Mouse::CursorPosition(float x, float y) noexcept {
    float data[] = { x, y, xPos_ - x, yPos_ - y };
    cursorCmd_.Extend(data);
    xPos_ = x;
    yPos_ = y;  
    return cursorCmd_;
}

cmd::Instruction Mouse::Button(int button, int action, int mods) noexcept {
    float data[] = { xPos_, yPos_, 0.0f, 0.0f };
    buttonCmds_[button].Extend(data);
    return buttonCmds_[button];
}

void Mouse::SetButtonCommand(const cmd::Instruction& command, int key) noexcept {
    buttonCmds_[key] = command;
}
void Mouse::SetButtonCommand(cmd::Instruction&& command, int key) noexcept {
    buttonCmds_[key] = std::move(command);
}

void Mouse::SetCursorCommand(cmd::Instruction&& command) noexcept {
    cursorCmd_ = std::move(command);
}

} // input
