#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {

class Exception {
    const char *message_;
    VkResult result_;

public:
    Exception(const char *message, VkResult result) : message_(message)
        , result_(result) {}

    const char* What() const noexcept { return message_; }
    VkResult Result() const noexcept { return result_; }
};

}
