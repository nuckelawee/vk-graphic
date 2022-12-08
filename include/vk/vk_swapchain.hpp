#pragma once

#include "vk_device.hpp"

namespace vk {

class Swapchain {

    VkSwapchainKHR swapchain_;

    VkFormat imageFormat_;
    VkExtent2D extent_;

    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;

public:

    VkResult CreateSwapchain(const Device& device, const Surface& surface);
    VkResult CreateImages(const Device& device);
    VkResult CreateImageViews();
    VkResult CreateFramebuffers();

    void Destroy(const Device& device);

    VkSwapchainKHR Access() const { return swapchain_; }
    VkFormat AccessImageFormat() const { return imageFormat_; }
    VkExtent2D AccessExtent() const { return extent_; }
private:

    int32_t ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>& 
        formats) const;
    int32_t ChooseSuitablePresent(const std::vector<VkPresentModeKHR>& 
        presentModes) const;
    VkExtent2D ChooseSuitableExtent(const VkSurfaceCapabilitiesKHR&
        capabilities) const;

public:
    Swapchain() {}
    ~Swapchain() {}

};

} //vk
