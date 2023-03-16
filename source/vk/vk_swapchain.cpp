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

VkExtent2D Swapchain::ChooseSuitableExtent(const Device& device
    , Surface& surface) const {

    VkSurfaceCapabilitiesKHR capabilities = surface.Capabilities(
        device.AccessGpu()).capabilities;
    if(capabilities.currentExtent.width == UINT32_MAX) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Screen coordinates don't correspond "\
            "to pixels", "Extent choosing");
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(&surface.AccessGLFW(), &width, &height);
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

void Swapchain::Create(const Device& device, Surface& surface) {
    SurfaceDetails surfaceCapabilities = surface.Capabilities(device.AccessGpu());

    QueueFamilies queueFamilies = device.AccessQueues();
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    int32_t index;

    index = ChooseSuitableFormat(surfaceCapabilities.formats);
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, index == -1)
        , "Failed to find suitable surface format"
        , "Swapchain creation");
    surfaceFormat = surfaceCapabilities.formats[index];

    index = ChooseSuitablePresent(
        surfaceCapabilities.presentModes);
    if(index == -1) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Failed to find suitable present mode"
            , "Swapchain creation");
        index = 0;
    }
    presentMode = surfaceCapabilities.presentModes[index];

    extent_ = ChooseSuitableExtent(device, surface);
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
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, result != VK_SUCCESS)
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
    , const GraphicPipeline& pipeline, const DataLoader& dataLoader) {

    framebuffers_.resize(imageViews_.size());
    for(size_t i = 0; i < framebuffers_.size(); i++) {
        uint32_t attachmentCount = 2;
        VkImageView attachments[attachmentCount]
            = { imageViews_[i], dataLoader.AccessDepthImage().imageView };


        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline.AccessRenderPass();
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent_.width;
        framebufferInfo.height = extent_.height;
        framebufferInfo.layers = 1;
        VkResult result = vkCreateFramebuffer(device.Access(), &framebufferInfo
            , nullptr, &(framebuffers_[i]));
        ErrorManager::Validate(result, "Framebuffer creation");
    }
}

void Swapchain::Recreate(const Device& device, Surface& surface
    , const GraphicPipeline& pipeline, DataLoader& dataLoader) {

    int width = 0, height = 0;
    glfwGetFramebufferSize(&surface.AccessGLFW(), &width, &height);
    while(width == 0 || height == 0) {
        glfwGetFramebufferSize(&surface.AccessGLFW(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.Access());

    CleanUp(device, dataLoader);
    Create(device, surface);
    CreateFramebuffers(device, pipeline, dataLoader);

}
     
VkViewport Swapchain::Viewport() const {
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent_.width;
    viewport.height = extent_.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

VkRect2D Swapchain::Scissor() const {
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = extent_;
    return scissor;
}

void Swapchain::CleanUp(const Device& device, DataLoader& dataLoader) {
    Image& image = dataLoader.AccessDepthImage();
    vkDestroyImageView(device.Access(), image.imageView, nullptr);
    vkDestroyImage(device.Access(), image.image, nullptr);
    vkFreeMemory(device.Access(), image.memory, nullptr); 

    for(auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device.Access(), framebuffer, nullptr);
    }
    for(auto imageView : imageViews_) {
        vkDestroyImageView(device.Access(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(device.Access(), swapchain_, nullptr);
}

} // vk
