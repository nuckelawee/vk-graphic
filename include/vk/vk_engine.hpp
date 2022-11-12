#pragma once

#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"
#include "glfw_window.hpp"
#include "swapchain.hpp"

namespace vk {

class VulkanEngine {
    VulkanLayersAndExtensions attachments_;
    VulkanInstance instance_;
    VulkanDevice device_;
    Swapchain swapchain_;

public:

    void Init(AppSetting& setting, Window& window); 
    void Update(AppSetting& setting, Window& window);
    void Terminate(Window& window);

    VulkanEngine operator=(const VulkanEngine& engine) = delete;
    VulkanEngine(const VulkanEngine& engine) = delete;
    VulkanEngine() {}
    ~VulkanEngine() {}

}; 

} //vk
