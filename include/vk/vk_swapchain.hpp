#pragma once

#include <algorithm>
#include "vk_image_builder.hpp"

class Surface;

namespace vk {

class Swapchain {

    Device device_;
    VkSwapchainKHR swapchain_;

    VkFormat imageFormat_;
    VkExtent2D extent_;

    Image depthImage_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> imageViews_;
    std::vector<VkFramebuffer> framebuffers_;

public:

    void Create(const Device& device, Surface& surface, ImageBuilder& imageBuilder
        , Setting& setting);
    void CreateFramebuffers(VkRenderPass renderPass);
    void Recreate(const Device& device, Surface& surface
        , ImageBuilder& imageBuilder, Setting& setting, VkRenderPass renderPass);

    void CleanUp();

    const VkSwapchainKHR& Access() const { return swapchain_; }
    VkSwapchainKHR& Access() { return swapchain_; }

    VkFormat AccessImageFormat() const { return imageFormat_; }

    VkExtent2D AccessExtent() const { return extent_; }

    VkFramebuffer AccessFramebuffer(size_t index) const 
    { return framebuffers_[index]; }

private:
    void CreateImages(ImageBuilder& imageBuidler);

    int32_t ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>& 
        formats) const;
    int32_t ChooseSuitablePresent(const std::vector<VkPresentModeKHR>& 
        presentModes) const;
    VkExtent2D ChooseSuitableExtent(Surface& surface) const;

public:
    Swapchain() {}
    ~Swapchain() {}

};

} //vk
