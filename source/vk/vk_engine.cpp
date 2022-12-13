#include "vk/vk_engine.hpp"

namespace vk {

void Engine::Update(Surface& surface) {
    vkWaitForFences(device_.Access(), 1, &(inFlight_[currentFrame_].Access())
        , VK_TRUE, UINT64_MAX);
    vkResetFences(device_.Access(), 1, &(inFlight_[currentFrame_].Access()));

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device_.Access(), swapchain_.Access(),
        UINT64_MAX, imageAvailable_[currentFrame_].Access()
        , VK_NULL_HANDLE, &imageIndex);
    
    vkResetCommandBuffer(commandBuffers_.Access()[currentFrame_], 0);

    VkPipelineStageFlags waitStages[] = 
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore pSignalSemaphores[] = { renderFinished_[currentFrame_].Access() };
    VkSemaphore pWaitSemaphores[] = { imageAvailable_[currentFrame_].Access() };
        
    commandBuffers_.Record(device_, pipeline_, swapchain_, imageIndex
        , currentFrame_);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = pWaitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(commandBuffers_.Access()[currentFrame_]);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = pSignalSemaphores;
    
    VkResult result = vkQueueSubmit(device_.AccessQueues().graphic.queue
        , 1, &submitInfo, inFlight_[currentFrame_].Access());
    ErrorManager::Validate(result, "Drawing");
    
    VkSwapchainKHR swapchain = swapchain_.Access();
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = pSignalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(device_.AccessQueues().present.queue, &presentInfo);
    currentFrame_ = (imageIndex+1) % AppSetting::frames;
}

void Engine::Init(Surface& surface) {
    instance_.IncludeDefaultLayersAndExtensions(attachments_);
    instance_.Create(attachments_);

    surface.Create(instance_);

    device_.PickGpu(instance_, surface, attachments_);
    device_.SetQueueFamilies(surface);
    device_.CreateLogicalDevice(surface, attachments_);

#ifdef DEBUG
    LayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif

    swapchain_.Create(device_, surface);

    const std::vector<std::string> shadersPath = {
        "build/trivial_vert.spv",
        "build/trivial_frag.spv"
    };
    pipeline_.Create(device_, swapchain_, shadersPath);
    swapchain_.CreateFramebuffers(device_, pipeline_);

    commandPool_.Create(device_);
    commandBuffers_.Create(device_, commandPool_, AppSetting::frames);

    for(size_t i = 0; i < AppSetting::frames; i++) {
        imageAvailable_[i].Create(device_);
        renderFinished_[i].Create(device_);
        inFlight_[i].Create(device_);
    }
}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(device_.Access());

    for(size_t i = 0; i < AppSetting::frames; i++) {
        imageAvailable_[i].Destroy(device_);
        renderFinished_[i].Destroy(device_);
        inFlight_[i].Destroy(device_);
    }
    commandPool_.Destroy(device_);
    swapchain_.Destroy(device_);
    pipeline_.Destroy(device_);
    device_.Destroy();
    surface.Destroy(instance_);
}

} //vk
