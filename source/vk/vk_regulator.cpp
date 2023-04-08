#include "vk/vk_regulator.hpp"
#include "vk/vk_device.hpp"
#include "vk/vk_settings.hpp"
#include "vk/vk_exception.hpp"

namespace vk {

VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device);

void Regulator::Create(VkDevice device) {
    device_ = device;
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        imageAvailable_[i] = createSemaphore(device_);
        renderFinished_[i] = createSemaphore(device_);
        inFlight_[i] = createFence(device_);
    }
}

VkResult Regulator::BeginRender(VkSwapchainKHR swapchain) noexcept {

    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();

    vkWaitForFences(device_, 1, inFlight_.data() + currentFrame
        , VK_TRUE, UINT64_MAX);

    vkResetFences(device_, 1, inFlight_.data() + currentFrame);

    VkResult result = vkAcquireNextImageKHR(device_, swapchain, UINT64_MAX
        , imageAvailable_[currentFrame], VK_NULL_HANDLE
        , &vk::Settings::GetInstance().ImageIndex());
    return result; 
}

void Regulator::Sync(VkSubmitInfo& submitInfo
    , VkPresentInfoKHR& presentInfo) noexcept {

    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = imageAvailable_.data() + currentFrame;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = renderFinished_.data() + currentFrame;
 
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = renderFinished_.data() + currentFrame;
}

void Regulator::Destroy() {
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        vkDestroySemaphore(device_, imageAvailable_[i], nullptr);
        vkDestroySemaphore(device_, renderFinished_[i], nullptr);
        vkDestroyFence(device_, inFlight_[i], nullptr);
    }
}

VkFence Regulator::AccessFence(size_t currentFrame) noexcept
{ return inFlight_[currentFrame]; }

VkSemaphore createSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr
        , &semaphore);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create semaphore", result);
    }
    return semaphore;
}

VkFence createFence(VkDevice device) {
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFence fence;
    VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &fence);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create fence", result);
    }
    return fence;
}

} //vk
