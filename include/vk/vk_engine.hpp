#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_surface.hpp"
#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_command_buffer.hpp"

namespace vk {

class Engine {
    LayersAndExtensions attachments_;
    Instance instance_;
    Device device_;
    Swapchain swapchain_;
    GraphicPipeline pipeline_;
    CommandPool commandPool_;
    CommandBuffer commandBuffer_;

public:

    void Init(AppSetting& setting, Surface& surface); 
    void Update(AppSetting& setting, Surface& surface);
    void Terminate(Surface& window);

    Engine operator=(const Engine& engine) = delete;
    Engine(const Engine& engine) = delete;
    Engine() {}
    ~Engine() {}

}; 

} //vk
