#pragma once

#include "vulkan_device.hpp"

class Swapchain {

   VkSwapchainKHR swapchain_;

public:

    VkResult Create(const VulkanDevice& device, const VkSurfaceKHR& surface);

    void Destroy(const VkDevice& device);
private:

    int32_t ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>& 
        formats) const;
    int32_t ChooseSuitablePresent(const std::vector<VkPresentModeKHR>& 
        presentModes) const;
    VkExtent2D ChooseSuitableExtent(const VkSurfaceCapabilitiesKHR&
        capabilities) const;

    Swapchain() {}
    ~Swapchain() {}

};
