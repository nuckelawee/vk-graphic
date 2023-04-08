#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {

struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;

    void Destroy(VkDevice device);
};

}
