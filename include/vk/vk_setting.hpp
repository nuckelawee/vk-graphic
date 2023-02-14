#pragma once

#include <string>

namespace vk {

class Setting {
public:
    static constexpr unsigned int frames = 2;

private:
    const std::string engineName = "No engine";

    unsigned int currentFrame = 0;
    uint32_t imageIndex;

    VkClearValue clearValue = { 0.2f, 0.2f, 0.2f, 1.0f };


public:

    const std::string& Engine() const { return engineName; }

    unsigned int CurrentFrame() const { return currentFrame; }
    unsigned int& CurrentFrame() { return currentFrame; }
    uint32_t ImageIndex() const { return imageIndex; }
    uint32_t& ImageIndex() { return imageIndex; }

    const VkClearValue& ClearValue() const { return clearValue; }
    VkClearValue& ClearValue() { return clearValue; }
};

} //vk
