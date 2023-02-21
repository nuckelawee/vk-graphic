#include "input/input_mouse.hpp"

namespace input {

void Mouse::CursorPosition(float x, float y) {
    xPrevPos_ = xPos_;
    yPrevPos_ = yPos_;
    xPos_ = x;
    yPos_ = y;  
}

void Mouse::Button(int button, int action, int mods) {

}

float Mouse::XPosition() const
{ return xPos_; }
float Mouse::YPosition() const
{ return yPos_; }

float Mouse::DeltaXPos() const 
{ return xPrevPos_ - xPos_; }

float Mouse::DeltaYPos() const
{ return yPrevPos_ - yPos_; }

} // input
