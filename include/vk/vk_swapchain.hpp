#pragma once

#include <algorithm>

#include "vk_device.hpp"
#include "vk_image.hpp"

class Surface;

namespace vk {

class ImageBuilder;

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

    void Create(const Device& device, const Surface& surface
        , ImageBuilder& imageBuilder);

    void CreateFramebuffers(VkRenderPass renderPass);

    void Recreate(const Device& device, const Surface& surface
        , ImageBuilder& imageBuilder, VkRenderPass renderPass);

    void CleanUp() noexcept;

    VkSwapchainKHR Access() const noexcept;
    VkFormat AccessImageFormat() const noexcept;
    VkFramebuffer AccessFramebuffer(size_t index) const noexcept;

private:
    void CreateImages(ImageBuilder& imageBuidler);

    int32_t ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>& 
        formats) const noexcept;
    int32_t ChooseSuitablePresent(const std::vector<VkPresentModeKHR>& 
        presentModes) const noexcept;
    VkExtent2D ChooseSuitableExtent(
        const Surface& surface) const noexcept;

};

} //vk
