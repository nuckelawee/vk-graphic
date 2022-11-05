#include "vulkan_device.hpp"

void VulkanDevice::Create(const VulkanInstance& instance
    , VulkanLayersAndExtensions& attachments) {

}

void VulkanDevice::PickGpu(const VulkanInstance& instance,
    , std::function<bool>(VkPhysicalDevice) = VulkanDevice::DefaultGpu() {

}
    
    
