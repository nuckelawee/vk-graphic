#include "input/input_mouse.hpp"

namespace input {

void Mouse::CursorPosition(float x, float y) noexcept {
    xPrevPos_ = xPos_;
    yPrevPos_ = yPos_;
    xPos_ = x;
    yPos_ = y;  
}

void Mouse::Button(int button, int action, int mods) noexcept {

}

float Mouse::XPosition() const noexcept
{ return xPos_; }
float Mouse::YPosition() const noexcept
{ return yPos_; }

float Mouse::DeltaXPos() const noexcept
{ return xPrevPos_ - xPos_; }

float Mouse::DeltaYPos() const noexcept
{ return yPrevPos_ - yPos_; }

} // input
