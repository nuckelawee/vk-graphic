#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vk {

class Setting {
public:
    static constexpr unsigned int frames = 2;

private:
    VkClearValue clearValues_[2] = {
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f },
    };

    const std::string engineName_ = "No engine";

    VkExtent2D extent_;
    unsigned int currentFrame_ = 0;
    uint32_t imageIndex_;

public:

    const std::string& Engine() const { return engineName_; }

    const VkClearValue* ClearValues() const { return clearValues_; }
    VkClearValue* ClearValues() { return clearValues_; }

    const VkExtent2D& Extent() const { return extent_; }
    VkExtent2D& Extent() { return extent_; }

    unsigned int CurrentFrame() const { return currentFrame_; }
    unsigned int& CurrentFrame() { return currentFrame_; }

    uint32_t ImageIndex() const { return imageIndex_; }
    uint32_t& ImageIndex() { return imageIndex_; }

};

} //vk
