#pragma once

#include "vk_swapchain.hpp"

namespace vk {

class Regulator {
    
    VkSemaphore imageAvailable_[vk::Setting::frames];
    VkSemaphore renderFinished_[vk::Setting::frames];
    VkFence inFlight_[vk::Setting::frames];

private:

    void CreateSemaphore(const Device& device, VkSemaphore& semaphore);
    void CreateFence(const Device& device, VkFence& fence);
    
    void DestroySemaphore(const Device& device, VkSemaphore& semaphore);
    void DestroyFence(const Device& device, VkFence& fence);

public:

    void Create(const Device& device);
    void Destroy(const Device& device);

    VkResult BeginRender(const Device& device, const Swapchain& swapchain
        , Setting& setting);

    void Sync(const Setting& setting, VkSubmitInfo& submitInfo
        , VkPresentInfoKHR& presentInfo);

    const VkFence& AccessFence(size_t currentFrame) const;
    VkFence& AccessFence(size_t currentFrame);

    Regulator() {}
    ~Regulator() {}

};

} // vk
