#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

namespace vk {

class Settings {
    std::array<VkClearValue, 2> clearValues_ {{
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f }
    }};
    VkExtent2D extent_;
    unsigned int currentFrame_ = 0;
    uint32_t imageIndex_;

public:

    static constexpr const char* engineName = "No engine";
    static constexpr uint32_t frames = 2;

public:

    static Settings& GetInstance() noexcept;

    const std::array<VkClearValue, 2>& ClearValues() const noexcept;
    std::array<VkClearValue, 2> ClearValues() noexcept;

    const VkExtent2D& Extent() const noexcept;
    void ChangeExtent(VkExtent2D extent) noexcept;

    unsigned int CurrentFrame() const noexcept;
    unsigned int& CurrentFrame() noexcept;

    uint32_t ImageIndex() const noexcept;
    uint32_t& ImageIndex() noexcept;

    Settings(const Settings&) = delete;
    Settings& operator=(const Settings) = delete;

private:

    Settings() = default;
};

} 
