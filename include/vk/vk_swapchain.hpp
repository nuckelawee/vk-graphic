#pragma once

#include <algorithm>

#include "vk_device.hpp"
#include "glfw_window.hpp"

namespace vk {

class GraphicPipeline;

class Swapchain {

    VkSwapchainKHR swapchain_;

    VkFormat imageFormat_;
    VkExtent2D extent_;

    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;

public:

    void Create(const Device& device, Surface& surface);
    void CreateFramebuffers(const Device& device, const GraphicPipeline& pipeline
        , const DataLoader& dataLoader);
    void Recreate(const Device& device, Surface& surface
        , const GraphicPipeline& pipeline, DataLoader& dataLoader);

    void CleanUp(const Device& device, DataLoader& dataLoader);

    const VkSwapchainKHR& Access() const { return swapchain_; }
    VkSwapchainKHR& Access() { return swapchain_; }

    VkFormat AccessImageFormat() const { return imageFormat_; }

    VkExtent2D AccessExtent() const { return extent_; }

    VkFramebuffer AccessFramebuffer(size_t index) const 
    { return framebuffers_[index]; }

    VkViewport Viewport() const;
    VkRect2D Scissor() const;

private:
    void CreateImages(const Device& device);
    void CreateImageViews(const Device& device);

    int32_t ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>& 
        formats) const;
    int32_t ChooseSuitablePresent(const std::vector<VkPresentModeKHR>& 
        presentModes) const;
    VkExtent2D ChooseSuitableExtent(const Device& device
        , Surface& surface) const;

public:
    Swapchain() {}
    ~Swapchain() {}

};

} //vk
