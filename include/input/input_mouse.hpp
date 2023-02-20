#pragma once

namespace input {

class Context;

class Mouse {

    float xPos_;
    float yPos_;

    float deltaXPos_;
    float deltaYPos_;
    
public:

    void CursorPosition(Context& context, float x, float y);
    void Button(Context& context, int button, int action, int mods);

    Mouse() {}
    ~Mouse() {}

};

} // input
