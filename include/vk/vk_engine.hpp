#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_surface.hpp"
#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_command_buffer.hpp"
#include "vk_semaphore.hpp"
#include "vk_fence.hpp"

namespace vk {

class Engine {
    LayersAndExtensions attachments_;
    Instance instance_;
    Device device_;
    Swapchain swapchain_;
    GraphicPipeline pipeline_;
    CommandPool commandPool_;
    CommandBuffer commandBuffers_;

    Semaphore imageAvailable_[AppSetting::frames];
    Semaphore renderFinished_[AppSetting::frames];
    Fence inFlight_[AppSetting::frames];

    unsigned int currentFrame_ = 0;

public:

    void Init(Surface& surface); 
    void Update(Surface& surface);
    void Terminate(Surface& window);

private:
    
    uint32_t Acquire(Surface& surface);
    void Submit(VkSemaphore*, VkSemaphore*);
    void Present(Surface&, VkSemaphore*, uint32_t);

public:
    Engine operator=(const Engine& engine) = delete;
    Engine(const Engine& engine) = delete;
    Engine() {}
    ~Engine() {}

}; 

} //vk
