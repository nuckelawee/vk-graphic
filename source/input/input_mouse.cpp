#include "input/input_mouse.hpp"
#include "input/input_context.hpp"

namespace input {

void Mouse::CursorPosition(Context& context, float x, float y) {
    deltaXPos_ = x - xPos_;
    deltaYPos_ = y - yPos_;
    xPos_ = x;
    yPos_ = y;  
//    context.UpdateMousePosition(x, y, deltaXPos_, deltaYPos_);
}

void Mouse::Button(Context& context, int button, int action, int mods) {
//    context.UpdateMouseButton(button);
}

} // input
