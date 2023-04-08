#include "vk/vk_surface.hpp"
#include "vk/vk_swapchain.hpp"
#include "vk/vk_exception.hpp"
#include "vk/vk_settings.hpp"
#include "vk/vk_image_builder.hpp"

namespace vk {

int32_t Swapchain::ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>&
    formats) const noexcept {

    for(size_t i = 0; i < formats.size(); i++) {
        if(formats[i].format == VK_FORMAT_B8G8R8A8_SRGB
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        { return i; }    
    }
    return -1; 
} 

int32_t Swapchain::ChooseSuitablePresent(const std::vector<VkPresentModeKHR>&
    presentModes) const noexcept {
    
    for(size_t i = 0; i < presentModes.size(); i++) {
        if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        { return i; }
    }
    return -1;
}

VkExtent2D Swapchain::ChooseSuitableExtent(
    const Surface& surface) const noexcept {

    VkSurfaceCapabilitiesKHR capabilities = surface.Capabilities(
        device_.AccessGpu()).capabilities;
    if(capabilities.currentExtent.width == UINT32_MAX) {
        std::cerr << "Screen coordinates don't correspond to pixels";
        return capabilities.currentExtent;
    }
    int width, height;
    surface.GetFramebufferSize(width, height);
    VkExtent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width
        , capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height
        , capabilities.maxImageExtent.height);
    return extent;
}

void Swapchain::Create(const Device& device, const Surface& surface
    , ImageBuilder& imageBuilder) {

    device_ = device;
    SurfaceDetails surfaceDetails = surface.Capabilities(device_.AccessGpu());

    QueueFamilies queueFamilies = device_.AccessQueues();
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    int32_t index;

    index = ChooseSuitableFormat(surfaceDetails.formats);
    if(index == -1) {
        throw std::runtime_error("Failed to find suitable surface format");
    }
    surfaceFormat = surfaceDetails.formats[index];

    index = ChooseSuitablePresent(surfaceDetails.presentModes);
    if(index == -1) {
        throw std::runtime_error("Failed to find suitable present mode");
    }

    presentMode = surfaceDetails.presentModes[index];

    extent_ = ChooseSuitableExtent(surface);
    vk::Settings::GetInstance().ChangeExtent(extent_);

    imageFormat_ = surfaceFormat.format;
    
    VkSwapchainCreateInfoKHR swapchainInfo {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface.Access();
    swapchainInfo.imageFormat = imageFormat_;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent_;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.minImageCount = surfaceDetails.capabilities.minImageCount;

    uint32_t pIndices[2] = { 
        queueFamilies.graphic.index.value(),
        queueFamilies.present.index.value()
    };
    if(queueFamilies.graphic != queueFamilies.present) {
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = pIndices;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.preTransform
        = surfaceDetails.capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_FALSE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkResult result = vkCreateSwapchainKHR(device_.Access(), &swapchainInfo
        , nullptr, &swapchain_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create swapchain", result);
    }
    
    CreateImages(imageBuilder);
}

void Swapchain::CreateImages(ImageBuilder& imageBuilder) {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device_.Access(), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    VkResult result = vkGetSwapchainImagesKHR(device_.Access(), swapchain_
        , &imageCount, images_.data());
    if(result != VK_SUCCESS) {
        throw Exception("Failed to get swapchain images", result);
    }

    imageViews_.resize(imageCount);
    
    for(size_t i = 0; i < imageCount; i++) {
        imageViews_[i] = ImageBuilder::CreateImageView(device_.Access()
            , images_[i], imageFormat_, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    depthImage_ = imageBuilder.BuildDepthImage(extent_);
}

void Swapchain::CreateFramebuffers(VkRenderPass renderPass) {
    framebuffers_.resize(imageViews_.size());
    for(size_t i = 0; i < framebuffers_.size(); i++) {
        uint32_t attachmentCount = 2;
        VkImageView attachments[attachmentCount]
            = { imageViews_[i], depthImage_.view };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent_.width;
        framebufferInfo.height = extent_.height;
        framebufferInfo.layers = 1;
        VkResult result = vkCreateFramebuffer(device_.Access(), &framebufferInfo
            , nullptr, &(framebuffers_[i]));
        if(result != VK_SUCCESS) {
            throw Exception("Failed to create framebuffer", result);
        }
    }
}

void Swapchain::Recreate(const Device& device, const Surface& surface
    , ImageBuilder& imageBuilder, VkRenderPass renderPass) {

    int width = 0, height = 0;
    surface.GetFramebufferSize(width, height);
    while(width == 0 || height == 0) {
        surface.GetFramebufferSize(width, height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device_.Access());

    CleanUp();
    Create(device, surface, imageBuilder);
    CreateFramebuffers(renderPass);
}

void Swapchain::CleanUp() noexcept {
    VkDevice device = device_.Access();
    vkDestroyImageView(device, depthImage_.view, nullptr);
    vkDestroyImage(device, depthImage_.image, nullptr);
    vkFreeMemory(device, depthImage_.memory, nullptr); 

    for(auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    for(auto imageView : imageViews_) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain_, nullptr);
}

VkSwapchainKHR Swapchain::Access() const noexcept
{ return swapchain_; }

VkFormat Swapchain::AccessImageFormat() const noexcept
{ return imageFormat_; }

VkFramebuffer Swapchain::AccessFramebuffer(size_t index) const noexcept
{ return framebuffers_[index]; }

} // vk
