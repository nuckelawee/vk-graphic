#include "vk/swapchain.hpp"

namespace vk {

int32_t Swapchain::ChooseSuitableFormat(const std::vector<VkSurfaceFormatKHR>&
    formats) const {

    std::cout << "Swapchain formats: " << formats.size() << '\n';
    for(size_t i = 0; i < formats.size(); i++) {
        if(formats[i].format == VK_FORMAT_B8G8R8A8_SRGB
            && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        { return i; }    
    }
    return -1; 
} 

int32_t Swapchain::ChooseSuitablePresent(const std::vector<VkPresentModeKHR>&
    presentModes) const {
    
    std::cout << "Swapchain presents: " << presentModes.size() << '\n';
    for(size_t i = 0; i < presentModes.size(); i++) {
        if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        { return i; }
    }
    return -1;
}

VkExtent2D Swapchain::ChooseSuitableExtent(const VkSurfaceCapabilitiesKHR&
    capabilities) const {
#ifdef DEBUG
    if(capabilities.currentExtent.width != UINT32_MAX) {
        std::cerr << "\nWARNING [ Extent choosing ]\n----> "\
            "screen coordinates don't correspond to pixels\n\n";
    }
#endif
    return capabilities.currentExtent;
}

VkResult Swapchain::Create(const VulkanDevice& device
    , const VkSurfaceKHR& surface) {

    SurfaceDetails surfaceCapabilities = Window::SurfaceCapabilities(device.AccessGpu()
        , surface);

    QueueFamilies queueFamilies = device.AccessQueues();
    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;
    int32_t index;

    index = ChooseSuitableFormat(surfaceCapabilities.formats);
    if(index == -1) {
#ifdef DEBUG
        std::cerr << "\nERROR [ Swapchain creation ]\n----> "\
            "Failed to find suitable surface format\n\n";
        exit(EXIT_FAILURE);
#endif    
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    format = surfaceCapabilities.formats[index];

    index = ChooseSuitablePresent(
        surfaceCapabilities.presentModes);
    if(index == -1) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ Swapchain creation ]\n----> "\
            "Failed to find suitable present mode\n\n";
        index = 0;
#endif
    }
    presentMode = surfaceCapabilities.presentModes[index];

    extent = ChooseSuitableExtent(surfaceCapabilities.capabilities);
    
    VkSwapchainCreateInfoKHR swapchainInfo {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.imageFormat = format.format;
    swapchainInfo.imageColorSpace = format.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.minImageCount = surfaceCapabilities.capabilities.maxImageCount;

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
#ifdef DEBUG
    if(result != VK_SUCCESS) {
        std::cerr << "\nERROR [ Swapchain creation ]\n---> "\
            "Failed to create swapchain\n\n";
        exit(EXIT_FAILURE);
#endif
    }
    return result;
}

void Swapchain::Destroy(const VkDevice& device) {
    vkDestroySwapchainKHR(device, swapchain_, nullptr);
}

}
