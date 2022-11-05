#pragma once

class VulkanDevice {
    VkPhysicalDevice gpu;
    VkDevice device;

public:

    void Create(VulkanLayersAndExtensions& attachments);

    VkPhysicalDevice& AccessGpu() { return gpu; }
    VkDevice& Access() { return device; }

    VulkanDevice();
    ~VulkanDevice();

private:
    void CreatePhysicalDevice();
    void CreateLogicalDevice();
    
};
