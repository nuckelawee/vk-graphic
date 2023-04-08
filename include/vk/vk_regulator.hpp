#pragma once

#include <array>
#include "vk_settings.hpp"

namespace vk {

class Device;

class Regulator {
    
    std::array<VkSemaphore, vk::Settings::frames> imageAvailable_;
    std::array<VkSemaphore, vk::Settings::frames> renderFinished_;
    std::array<VkFence, vk::Settings::frames> inFlight_;
    VkDevice device_;

public:

    void Create(VkDevice device);
    void Destroy();

    VkResult BeginRender(VkSwapchainKHR swapchain) noexcept;

    void Sync(VkSubmitInfo& submitInfo, VkPresentInfoKHR& presentInfo) noexcept;

    VkFence AccessFence(size_t currentFrame) noexcept;

};

} // vk
