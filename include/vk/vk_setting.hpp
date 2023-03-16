#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vk {

class Setting {
public:
    static constexpr unsigned int frames = 2;

private:
    const std::string engineName = "No engine";

    unsigned int currentFrame = 0;
    uint32_t imageIndex;

    VkClearValue clearValues[2] = {
        { 0.2f, 0.2f, 0.2f, 1.0f },
        { 1.0f, 0.0f },
    };

public:

    const std::string& Engine() const { return engineName; }

    unsigned int CurrentFrame() const { return currentFrame; }
    unsigned int& CurrentFrame() { return currentFrame; }
    uint32_t ImageIndex() const { return imageIndex; }
    uint32_t& ImageIndex() { return imageIndex; }

    const VkClearValue* ClearValues() const { return clearValues; }
    VkClearValue* ClearValues() { return clearValues; }
};

} //vk
