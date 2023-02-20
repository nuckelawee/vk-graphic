#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace input {

class Context;

class Keyboard {

    bool pKeys_[1024];
    
public:

    void KeyInput(Context& context, int key, int scancode, int action
        , int modes);

    const bool* Keys() const;

    Keyboard() {}
    ~Keyboard() {}
};

} // input
