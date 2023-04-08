#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace input {

class Keyboard {

    std::vector<bool> keys_;

public:

    Keyboard();

    void KeyInput(int key, int scancode, int action
        , int modes) noexcept;

    const std::vector<bool>& Keys() const noexcept;

};

} // input
