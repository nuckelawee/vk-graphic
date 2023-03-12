#pragma once

namespace input {

class Mouse {

    float xPos_ = 0.0f;
    float yPos_ = 0.0f;

    float xPrevPos_ = 0.0f;
    float yPrevPos_ = 0.0f;

    //float deltaXPos_;
    //float deltaYPos_;
    
public:

    void CursorPosition(float x, float y);
    void Button(int button, int action, int mods);

    float XPosition() const;
    float YPosition() const;

    float DeltaXPos() const;
    float DeltaYPos() const;

    Mouse() {}
    ~Mouse() {}

};

} // input
