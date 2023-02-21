#pragma once

namespace input {

class Mouse {

    float xPos_;
    float yPos_;

    float xPrevPos_;
    float yPrevPos_;

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
