#include "vk/vk_graphic_pipeline.hpp"

namespace vk {

int32_t Swapchain::ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>&
    formats) const {

    for(size_t i = 0; i < formats.size(); i++) {
        if(formats[i].format == VK_FORMAT_B8G8R8A8_SRGB
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        { return i; }    
    }
    return -1; 
} 

int32_t Swapchain::ChooseSuitablePresent(const std::vector<VkPresentModeKHR>&
    presentModes) const {
    
    for(size_t i = 0; i < presentModes.size(); i++) {
        if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        { return i; }
    }
    return -1;
}

VkExtent2D Swapchain::ChooseSuitableExtent(const VkSurfaceCapabilitiesKHR&
    capabilities) const {
    ErrorManager::Validate(Error(WARNING, capabilities.currentExtent.width
        != UINT32_MAX), "Screen coordinates don't correspond to pixels"
        , "Extent choosing");
    return capabilities.currentExtent;
}

void Swapchain::Create(const Device& device, const Surface& surface) {
    SurfaceDetails surfaceCapabilities = surface.Capabilities(device.AccessGpu());

    QueueFamilies queueFamilies = device.AccessQueues();
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    int32_t index;

    index = ChooseSuitableFormat(surfaceCapabilities.formats);
    ErrorManager::Validate(Error(UNSOLVABLE, index == -1)
        , "Failed to find suitable surface format"
        , "Swapchain creation");
    surfaceFormat = surfaceCapabilities.formats[index];

    index = ChooseSuitablePresent(
        surfaceCapabilities.presentModes);
    if(index == -1) {
        ErrorManager::Validate(WARNING, "Failed to find suitable present mode"
            , "Swapchain creation");
        index = 0;
    }
    presentMode = surfaceCapabilities.presentModes[index];

    extent_ = ChooseSuitableExtent(surfaceCapabilities.capabilities);
    imageFormat_ = surfaceFormat.format;
    
    VkSwapchainCreateInfoKHR swapchainInfo {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface.Access();
    swapchainInfo.imageFormat = imageFormat_;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent_;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.minImageCount = surfaceCapabilities.capabilities.minImageCount;

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
        = surfaceCapabilities.capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_FALSE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkResult result = vkCreateSwapchainKHR(device.Access(), &swapchainInfo
        , nullptr, &swapchain_);
    ErrorManager::Validate(Error(UNSOLVABLE, result != VK_SUCCESS)
        , "Failed to create swapchain", "Swapchain creation");
    
    CreateImages(device);
    CreateImageViews(device);
}

void Swapchain::CreateImages(const Device& device) {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device.Access(), swapchain_, &imageCount, nullptr);
    images_.resize(imageCount);
    vkGetSwapchainImagesKHR(device.Access(), swapchain_, &imageCount, images_.data());
}

void Swapchain::CreateImageViews(const Device& device) {
    imageViews_.resize(images_.size());
    
    for(size_t i = 0; i < images_.size(); i++) {
        VkImageViewCreateInfo imageViewInfo {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = images_[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = imageFormat_;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        VkResult result = vkCreateImageView(device.Access(), &imageViewInfo
            , nullptr, &(imageViews_[i]));
        ErrorManager::Validate(result, "Image view creation");
    }
}

void Swapchain::CreateFramebuffers(const Device& device
    , const GraphicPipeline& pipeline) {

    framebuffers_.resize(imageViews_.size());
    for(size_t i = 0; i < framebuffers_.size(); i++) {
        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline.AccessRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &(imageViews_[i]);
        framebufferInfo.width = extent_.width;
        framebufferInfo.height = extent_.height;
        framebufferInfo.layers = 1;
        VkResult result = vkCreateFramebuffer(device.Access(), &framebufferInfo
            , nullptr, &(framebuffers_[i]));
        ErrorManager::Validate(result, "Framebuffer creation");
    }
}

void Swapchain::Destroy(const Device& device) {
    for(auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device.Access(), framebuffer, nullptr);
    }
    for(auto imageView : imageViews_) {
        vkDestroyImageView(device.Access(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(device.Access(), swapchain_, nullptr);
}

} // vk