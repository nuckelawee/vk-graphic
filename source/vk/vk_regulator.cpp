#include "vk/vk_regulator.hpp"

namespace vk {

void Regulator::Create(const Device& device) {
    for(size_t i = 0; i < vk::Setting::frames; i++) {
        CreateSemaphore(device, imageAvailable_[i]);
        CreateSemaphore(device, renderFinished_[i]);
        CreateFence(device, inFlight_[i]);
    }
}

VkResult Regulator::BeginRender(const Device& device, const Swapchain& swapchain
    , Setting& setting) {

    uint32_t currentFrame = setting.CurrentFrame();

    vkWaitForFences(device.Access(), 1, inFlight_ + currentFrame
        , VK_TRUE, UINT64_MAX);

    vkResetFences(device.Access(), 1, inFlight_ + currentFrame);

    VkResult result = vkAcquireNextImageKHR(device.Access(), swapchain.Access()
        , UINT64_MAX, imageAvailable_[currentFrame]
        , VK_NULL_HANDLE, &setting.ImageIndex());
    return result; 
}

void Regulator::Sync(const Setting& setting, VkSubmitInfo& submitInfo
    , VkPresentInfoKHR& presentInfo) {

    uint32_t currentFrame = setting.CurrentFrame();

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = imageAvailable_ + currentFrame;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = renderFinished_ + currentFrame;
 
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = renderFinished_ + currentFrame;
}

void Regulator::Destroy(const Device& device) {
    for(size_t i = 0; i < vk::Setting::frames; i++) {
        DestroySemaphore(device, imageAvailable_[i]);
        DestroySemaphore(device, renderFinished_[i]);
        DestroyFence(device, inFlight_[i]);
    }
}

const VkFence& Regulator::AccessFence(size_t currentFrame) const
{ return inFlight_[currentFrame]; }

VkFence& Regulator::AccessFence(size_t currentFrame)
{ return inFlight_[currentFrame]; }

void Regulator::CreateSemaphore(const Device& device, VkSemaphore& semaphore) {
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkResult result = vkCreateSemaphore(device.Access()
        , &semaphoreInfo, nullptr, &semaphore);
    ErrorManager::Validate(result, "Semaphore creation");
}

void Regulator::CreateFence(const Device& device, VkFence& fence) {
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkResult result = vkCreateFence(device.Access(), &fenceInfo
        , nullptr, &fence);
    ErrorManager::Validate(result, "Fence creation");
}

void Regulator::DestroySemaphore(const Device& device, VkSemaphore& semaphore) {
    vkDestroySemaphore(device.Access(), semaphore, nullptr); 
}

void Regulator::DestroyFence(const Device& device, VkFence& fence) {
    vkDestroyFence(device.Access(), fence, nullptr); 
}

} //vk
